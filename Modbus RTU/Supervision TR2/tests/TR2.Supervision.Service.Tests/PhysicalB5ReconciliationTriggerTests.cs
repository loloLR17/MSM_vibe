using TR2.Application;
using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class PhysicalB5ReconciliationTriggerTests
{
    [Fact]
    public async Task CompatibleSessionQueuesRecoveredAmbiguousOnlyOnceAndNeverWrites()
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
            await operations.QueueCommandAsync(endpoint, "recovered-ambiguous", new B5CommandIntent(5, 0, 0, 0, 0), now);
            var coordinator = composition.CommandCoordinatorRegistry.Get(deviceId);
            await coordinator.MarkSubmittedAsync(now.AddMilliseconds(1));
            await coordinator.MarkAmbiguousAsync(now.AddMilliseconds(2));

            var runner = new PhysicalB5LifecycleWorkRunner(composition, operations);
            runner.ObserveCompatibleSession(endpoint, now.AddMilliseconds(3));
            runner.ObserveCompatibleSession(endpoint, now.AddMilliseconds(4));

            var due = now.AddMilliseconds(20);
            var work = composition.BusWorkScheduler.BeginNext(endpoint.Bus, due)!;
            Assert.Equal(BusWorkKind.TransactionReconciliation, work.Kind);
            Assert.Null(composition.BusWorkScheduler.BeginNext(endpoint.Bus, due));
            Assert.Empty(transport.Writes);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task DifferentDeviceAtEndpointDoesNotQueueRecoveredTransaction()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var transport = new ScriptedTransport();
            var composition = Compose(databasePath, transport);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            var originalDeviceId = new DeviceId(42);
            composition.FleetRegistry.SetSession(TR2Session.CreateCompatible(endpoint, new TR2Device(originalDeviceId)));

            var operations = new SupervisionOperationalFacade(composition);
            var now = new DateTimeOffset(2026, 9, 10, 10, 0, 0, TimeSpan.Zero);
            await operations.QueueCommandAsync(endpoint, "ambiguous-other-device", new B5CommandIntent(5, 0, 0, 0, 0), now);
            var coordinator = composition.CommandCoordinatorRegistry.Get(originalDeviceId);
            await coordinator.MarkSubmittedAsync(now.AddMilliseconds(1));
            await coordinator.MarkAmbiguousAsync(now.AddMilliseconds(2));

            composition.FleetRegistry.SetSession(TR2Session.CreateCompatible(endpoint, new TR2Device(new DeviceId(99))));
            var runner = new PhysicalB5LifecycleWorkRunner(composition, operations);
            runner.ObserveCompatibleSession(endpoint, now.AddMilliseconds(3));

            var first = composition.BusWorkScheduler.BeginNext(endpoint.Bus, now.AddSeconds(1));
            Assert.NotNull(first);
            Assert.Equal(BusWorkKind.CommandTransaction, first!.Kind);
            Assert.Empty(transport.Writes);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    private static SupervisionRuntimeComposition Compose(string databasePath, ScriptedTransport transport)
    {
        var escaped = databasePath.Replace("\\", "\\\\");
        var configuration = RuntimeConfigurationLoader.Parse(
            $$"""
            {
              "persistence": { "databasePath": "{{escaped}}" },
              "buses": [{
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
              }],
              "b5": {
                "postSubmitPollIntervalMilliseconds": 10,
                "postSubmitTimeoutMilliseconds": 50,
                "reconciliationIntervalMilliseconds": 10
              }
            }
            """,
            Path.GetDirectoryName(databasePath)!);

        return SupervisionRuntimeCompositionRoot.Compose(configuration, new FakeFactory(transport));
    }

    private static string NewDatabasePath() => Path.Combine(Path.GetTempPath(), $"tr2-supervision-h3b4-{Guid.NewGuid():N}.db");

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
        public ValueTask<IModbusBusConnection> OpenAsync(string busId, ModbusSerialConnectionSettings settings, CancellationToken cancellationToken = default) =>
            ValueTask.FromResult<IModbusBusConnection>(new FakeConnection(busId, transport));
    }

    private sealed class FakeConnection(string busId, ScriptedTransport transport) : IModbusBusConnection
    {
        public string BusId => busId;
        public IRegisterTransport RegisterTransport => transport;
        public IRegisterWriteTransport RegisterWriteTransport => transport;
        public ValueTask DisposeAsync() => ValueTask.CompletedTask;
    }

    private sealed class ScriptedTransport : IRegisterTransport, IRegisterWriteTransport
    {
        public List<(ushort StartAddress, ushort[] Values)> Writes { get; } = [];
        public ValueTask<ushort[]> ReadRegistersAsync(string busId, byte unitAddress, ushort startAddress, ushort registerCount, CancellationToken cancellationToken = default) =>
            ValueTask.FromResult(new ushort[registerCount]);
        public ValueTask WriteRegistersAsync(string busId, byte unitAddress, ushort startAddress, IReadOnlyList<ushort> values, CancellationToken cancellationToken = default)
        {
            Writes.Add((startAddress, values.ToArray()));
            return ValueTask.CompletedTask;
        }
    }
}
