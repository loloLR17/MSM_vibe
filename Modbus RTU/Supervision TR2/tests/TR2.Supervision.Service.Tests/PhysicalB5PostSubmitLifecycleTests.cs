using TR2.Application;
using TR2.Domain;
using TR2.Protocol;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class PhysicalB5PostSubmitLifecycleTests
{
    [Fact]
    public async Task SubmittedCommandIsResolvedOnlyByTerminalB5Evidence()
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
            var now = new DateTimeOffset(2026, 9, 10, 10, 0, 0, TimeSpan.Zero);
            var queued = await operations.QueueCommandAsync(
                endpoint,
                "request-terminal",
                new B5CommandIntent(5, 1, 2, 3, 0),
                now);

            var runner = new PhysicalB5LifecycleWorkRunner(composition, operations);
            var commandWork = composition.BusWorkScheduler.BeginNext(endpoint.Bus, now)!;
            await runner.ExecuteAsync(commandWork, now);

            Assert.Equal(CommandTransactionState.Submitted,
                composition.CommandCoordinatorRegistry.Get(deviceId).ActiveTransaction!.State);
            Assert.Equal(2, transport.Writes.Count);

            var registers = new ushort[B5CommandState.RegisterCount];
            registers[15] = queued.Request.TransactionId;
            registers[16] = 4;
            transport.B5Registers = registers;

            var monitorAt = now + TimeSpan.FromMilliseconds(10);
            var monitorWork = composition.BusWorkScheduler.BeginNext(endpoint.Bus, monitorAt)!;
            Assert.Equal(BusWorkKind.CommandPostSubmitMonitoring, monitorWork.Kind);
            await runner.ExecuteAsync(monitorWork, monitorAt);

            Assert.Null(composition.CommandCoordinatorRegistry.Get(deviceId).ActiveTransaction);
            Assert.Equal(2, transport.Writes.Count);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task SubmittedCommandBecomesAmbiguousAtConfiguredTimeoutWithoutReplay()
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
            var now = new DateTimeOffset(2026, 9, 10, 10, 0, 0, TimeSpan.Zero);
            await operations.QueueCommandAsync(
                endpoint,
                "request-timeout",
                new B5CommandIntent(5, 1, 2, 3, 0),
                now);

            var runner = new PhysicalB5LifecycleWorkRunner(composition, operations);
            await runner.ExecuteAsync(composition.BusWorkScheduler.BeginNext(endpoint.Bus, now)!, now);

            var firstMonitorAt = now + TimeSpan.FromMilliseconds(10);
            await runner.ExecuteAsync(
                composition.BusWorkScheduler.BeginNext(endpoint.Bus, firstMonitorAt)!,
                firstMonitorAt);

            Assert.Equal(CommandTransactionState.Submitted,
                composition.CommandCoordinatorRegistry.Get(deviceId).ActiveTransaction!.State);

            var timeoutAt = now + TimeSpan.FromMilliseconds(100);
            var timeoutWork = composition.BusWorkScheduler.BeginNext(endpoint.Bus, timeoutAt)!;
            await runner.ExecuteAsync(timeoutWork, timeoutAt);

            Assert.Equal(CommandTransactionState.Ambiguous,
                composition.CommandCoordinatorRegistry.Get(deviceId).ActiveTransaction!.State);
            Assert.Equal(2, transport.Writes.Count);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    private static SupervisionRuntimeComposition Compose(string databasePath, ScriptedTransport transport)
    {
        var escapedDatabasePath = databasePath.Replace("\\", "\\\\");
        var configuration = RuntimeConfigurationLoader.Parse(
            $$"""
            {
              "persistence": { "databasePath": "{{escapedDatabasePath}}" },
              "b5": {
                "postSubmitPollIntervalMilliseconds": 10,
                "postSubmitTimeoutMilliseconds": 100,
                "reconciliationIntervalMilliseconds": 50
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

        return SupervisionRuntimeCompositionRoot.Compose(configuration, new FakeFactory(transport));
    }

    private static string NewDatabasePath() =>
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s7h3b2-{Guid.NewGuid():N}.db");

    private static void DeleteDatabaseFiles(string databasePath)
    {
        foreach (var suffix in new[] { string.Empty, "-wal", "-shm" })
        {
            var path = databasePath + suffix;
            if (File.Exists(path)) File.Delete(path);
        }
    }

    private sealed class FakeFactory(ScriptedTransport transport) : IModbusBusConnectionFactory
    {
        public ValueTask<IModbusBusConnection> OpenAsync(
            string busId,
            ModbusSerialConnectionSettings settings,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromResult<IModbusBusConnection>(new FakeConnection(busId, transport));
    }

    private sealed class FakeConnection(string busId, ScriptedTransport transport) : IModbusBusConnection
    {
        public string BusId { get; } = busId;
        public IRegisterTransport RegisterTransport { get; } = transport;
        public IRegisterWriteTransport RegisterWriteTransport { get; } = transport;
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
            Assert.Equal(B5CommandState.StartAddress, startAddress);
            return ValueTask.FromResult(B5Registers.Take(registerCount).ToArray());
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
