using TR2.Application;
using TR2.Domain;
using TR2.Protocol;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class PhysicalB5ReconciliationLifecycleTests
{
    [Fact]
    public async Task TimedOutSubmittedCommandIsResolvedByLaterTerminalB5EvidenceWithoutReplay()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var transport = new ScriptedTransport();
            var composition = Compose(databasePath, transport);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            var deviceId = new DeviceId(42);
            composition.FleetRegistry.SetSession(TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));

            var operations = new SupervisionOperationalFacade(composition);
            var now = new DateTimeOffset(2026, 9, 10, 11, 0, 0, TimeSpan.Zero);
            var queued = await operations.QueueCommandAsync(
                endpoint,
                "request-reconcile-terminal",
                new B5CommandIntent(5, 1, 2, 3, 0),
                now);

            var runner = new PhysicalB5LifecycleWorkRunner(composition, operations);
            await runner.ExecuteAsync(composition.BusWorkScheduler.BeginNext(endpoint.Bus, now)!, now);
            Assert.Equal(2, transport.Writes.Count);

            var firstMonitorAt = now + TimeSpan.FromMilliseconds(10);
            await runner.ExecuteAsync(
                composition.BusWorkScheduler.BeginNext(endpoint.Bus, firstMonitorAt)!,
                firstMonitorAt);

            var timeoutAt = now + TimeSpan.FromMilliseconds(20);
            var timeoutWork = composition.BusWorkScheduler.BeginNext(endpoint.Bus, timeoutAt)!;
            Assert.Equal(BusWorkKind.CommandPostSubmitMonitoring, timeoutWork.Kind);
            await runner.ExecuteAsync(timeoutWork, timeoutAt);

            var coordinator = composition.CommandCoordinatorRegistry.Get(deviceId);
            Assert.Equal(CommandTransactionState.Ambiguous, coordinator.ActiveTransaction!.State);
            Assert.Equal(2, transport.Writes.Count);

            var terminal = new ushort[B5CommandState.RegisterCount];
            terminal[15] = queued.Request.TransactionId;
            terminal[16] = 4;
            transport.B5Registers = terminal;

            var reconcileAt = timeoutAt + TimeSpan.FromMilliseconds(30);
            var reconciliationWork = composition.BusWorkScheduler.BeginNext(endpoint.Bus, reconcileAt)!;
            Assert.Equal(BusWorkKind.TransactionReconciliation, reconciliationWork.Kind);
            await runner.ExecuteAsync(reconciliationWork, reconcileAt);

            Assert.Null(coordinator.ActiveTransaction);
            Assert.Equal(2, transport.Writes.Count);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task InsufficientReconciliationEvidenceKeepsAmbiguousAndQueuesAnotherReadOnlyObservation()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var transport = new ScriptedTransport();
            var composition = Compose(databasePath, transport);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            var deviceId = new DeviceId(42);
            composition.FleetRegistry.SetSession(TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));

            var operations = new SupervisionOperationalFacade(composition);
            var now = new DateTimeOffset(2026, 9, 10, 11, 30, 0, TimeSpan.Zero);
            await operations.QueueCommandAsync(
                endpoint,
                "request-reconcile-pending",
                new B5CommandIntent(5, 1, 2, 3, 0),
                now);

            var runner = new PhysicalB5LifecycleWorkRunner(composition, operations);
            await runner.ExecuteAsync(composition.BusWorkScheduler.BeginNext(endpoint.Bus, now)!, now);

            var firstMonitorAt = now + TimeSpan.FromMilliseconds(10);
            await runner.ExecuteAsync(
                composition.BusWorkScheduler.BeginNext(endpoint.Bus, firstMonitorAt)!,
                firstMonitorAt);

            var timeoutAt = now + TimeSpan.FromMilliseconds(20);
            await runner.ExecuteAsync(
                composition.BusWorkScheduler.BeginNext(endpoint.Bus, timeoutAt)!,
                timeoutAt);

            var reconcileAt = timeoutAt + TimeSpan.FromMilliseconds(30);
            var reconciliationWork = composition.BusWorkScheduler.BeginNext(endpoint.Bus, reconcileAt)!;
            Assert.Equal(BusWorkKind.TransactionReconciliation, reconciliationWork.Kind);
            await runner.ExecuteAsync(reconciliationWork, reconcileAt);

            var coordinator = composition.CommandCoordinatorRegistry.Get(deviceId);
            Assert.Equal(CommandTransactionState.Ambiguous, coordinator.ActiveTransaction!.State);
            Assert.Equal(2, transport.Writes.Count);

            var nextAt = reconcileAt + TimeSpan.FromMilliseconds(30);
            var next = composition.BusWorkScheduler.BeginNext(endpoint.Bus, nextAt)!;
            Assert.Equal(BusWorkKind.TransactionReconciliation, next.Kind);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    private static SupervisionRuntimeComposition Compose(
        string databasePath,
        ScriptedTransport transport)
    {
        var escapedDatabasePath = databasePath.Replace("\\", "\\\\");
        var configuration = RuntimeConfigurationLoader.Parse(
            $$"""
            {
              "persistence": { "databasePath": "{{escapedDatabasePath}}" },
              "b5": {
                "postSubmitPollIntervalMilliseconds": 10,
                "postSubmitTimeoutMilliseconds": 20,
                "reconciliationIntervalMilliseconds": 30
              },
              "buses": [
                {
                  "id": "bus-1",
                  "serial": {
                    "portName": "COM7",
                    "baudRate": 115200,
                    "dataBits": 8,
                    "parity": "None",
                    "stopBits": "One",
                    "responseTimeoutMilliseconds": 750
                  },
                  "endpoints": [1]
                }
              ]
            }
            """,
            Path.GetDirectoryName(databasePath)!);

        return SupervisionRuntimeCompositionRoot.Compose(
            configuration,
            new FakeFactory(transport));
    }

    private static string NewDatabasePath() =>
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s7h3b3-{Guid.NewGuid():N}.db");

    private static void DeleteDatabaseFiles(string databasePath)
    {
        foreach (var suffix in new[] { string.Empty, "-wal", "-shm" })
        {
            var path = databasePath + suffix;
            if (File.Exists(path))
            {
                File.Delete(path);
            }
        }
    }

    private sealed class FakeFactory : IModbusBusConnectionFactory
    {
        private readonly ScriptedTransport _transport;

        public FakeFactory(ScriptedTransport transport)
        {
            _transport = transport;
        }

        public ValueTask<IModbusBusConnection> OpenAsync(
            string busId,
            ModbusSerialConnectionSettings settings,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            return ValueTask.FromResult<IModbusBusConnection>(new FakeConnection(busId, _transport));
        }
    }

    private sealed class FakeConnection : IModbusBusConnection
    {
        public FakeConnection(string busId, ScriptedTransport transport)
        {
            BusId = busId;
            RegisterTransport = transport;
            RegisterWriteTransport = transport;
        }

        public string BusId { get; }
        public IRegisterTransport RegisterTransport { get; }
        public IRegisterWriteTransport RegisterWriteTransport { get; }
        public ValueTask DisposeAsync() => ValueTask.CompletedTask;
    }

    private sealed class ScriptedTransport : IRegisterTransport, IRegisterWriteTransport
    {
        public ushort[] B5Registers { get; set; } = new ushort[B5CommandState.RegisterCount];
        public List<(ushort StartAddress, ushort[] Values)> Writes { get; } = [];

        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            if (startAddress == B5CommandState.StartAddress)
            {
                return ValueTask.FromResult(B5Registers.Take(registerCount).ToArray());
            }

            return ValueTask.FromResult(new ushort[registerCount]);
        }

        public ValueTask WriteRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            IReadOnlyList<ushort> values,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            Writes.Add((startAddress, values.ToArray()));
            return ValueTask.CompletedTask;
        }
    }
}
