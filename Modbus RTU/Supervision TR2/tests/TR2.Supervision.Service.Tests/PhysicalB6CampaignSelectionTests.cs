using TR2.Application;
using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class PhysicalB6CampaignSelectionTests
{
    [Fact]
    public async Task CampaignSelectionWritesExactlyOnceThroughPhysicalTransport()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var transport = new RecordingTransport();
            var composition = Compose(databasePath, new FakeFactory(transport));
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            composition.FleetRegistry.SetSession(
                TR2Session.CreateCompatible(endpoint, new TR2Device(new DeviceId(42))));

            var operations = new SupervisionOperationalFacade(composition);
            var now = new DateTimeOffset(2026, 9, 9, 18, 0, 0, TimeSpan.Zero);
            var queued = operations.QueueCampaignSelection(endpoint, 7, now);
            var work = composition.BusWorkScheduler.BeginNext(endpoint.Bus, now)!;

            await new PhysicalPriorityWorkRunner(composition, operations).ExecuteAsync(work, now);

            var write = Assert.Single(transport.Writes);
            Assert.Equal(endpoint.Bus.Id, write.BusId);
            Assert.Equal(endpoint.Address.Value, write.Address);
            Assert.Equal((ushort)6003, write.StartAddress);
            Assert.Equal(new ushort[] { 7 }, write.Values);
            Assert.Equal(queued.Work.WorkId, work.WorkId);
            Assert.Null(composition.BusWorkScheduler.BeginNext(endpoint.Bus, now));
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task CampaignSelectionTimeoutIsContainedWithoutRetryOrDisconnect()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var transport = new RecordingTransport(ModbusTransportFailureKind.Timeout);
            var composition = Compose(databasePath, new FakeFactory(transport));
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            composition.FleetRegistry.SetSession(
                TR2Session.CreateCompatible(endpoint, new TR2Device(new DeviceId(43))));

            var operations = new SupervisionOperationalFacade(composition);
            var now = new DateTimeOffset(2026, 9, 9, 18, 1, 0, TimeSpan.Zero);
            operations.QueueCampaignSelection(endpoint, 3, now);
            var work = composition.BusWorkScheduler.BeginNext(endpoint.Bus, now)!;

            await new PhysicalPriorityWorkRunner(composition, operations).ExecuteAsync(work, now);

            Assert.Equal(1, transport.Attempts);
            Assert.True(composition.BusConnectionManager.TryGet(endpoint.Bus.Id, out _));
            Assert.Equal(TR2SessionState.Compatible, composition.FleetRegistry.GetSession(endpoint).State);
            Assert.Null(composition.BusWorkScheduler.BeginNext(endpoint.Bus, now));
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task CampaignSelectionIoFailureIsContainedWithoutRetryAndDisconnectsBus()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var transport = new RecordingTransport(ModbusTransportFailureKind.Io);
            var composition = Compose(databasePath, new FakeFactory(transport));
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            composition.FleetRegistry.SetSession(
                TR2Session.CreateCompatible(endpoint, new TR2Device(new DeviceId(44))));

            var operations = new SupervisionOperationalFacade(composition);
            var now = new DateTimeOffset(2026, 9, 9, 18, 2, 0, TimeSpan.Zero);
            operations.QueueCampaignSelection(endpoint, 5, now);
            var work = composition.BusWorkScheduler.BeginNext(endpoint.Bus, now)!;

            await new PhysicalPriorityWorkRunner(composition, operations).ExecuteAsync(work, now);

            Assert.Equal(1, transport.Attempts);
            Assert.False(composition.BusConnectionManager.TryGet(endpoint.Bus.Id, out _));
            Assert.Equal(TR2SessionState.Unidentified, composition.FleetRegistry.GetSession(endpoint).State);
            Assert.Null(composition.BusWorkScheduler.BeginNext(endpoint.Bus, now));
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task CampaignSelectionRequiresCompatibleSessionBeforeItIsQueued()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var composition = Compose(databasePath, new FakeFactory(new RecordingTransport()));
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            var operations = new SupervisionOperationalFacade(composition);
            var now = new DateTimeOffset(2026, 9, 9, 18, 3, 0, TimeSpan.Zero);

            Assert.Throws<InvalidOperationException>(() =>
                operations.QueueCampaignSelection(endpoint, 1, now));

            Assert.Null(composition.BusWorkScheduler.BeginNext(endpoint.Bus, now));
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    private static SupervisionRuntimeComposition Compose(
        string databasePath,
        IModbusBusConnectionFactory factory)
    {
        var escapedDatabasePath = databasePath.Replace("\\", "\\\\");
        var configuration = RuntimeConfigurationLoader.Parse(
            $$"""
            {
              "persistence": { "databasePath": "{{escapedDatabasePath}}" },
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

        return SupervisionRuntimeCompositionRoot.Compose(configuration, factory);
    }

    private static string NewDatabasePath() =>
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s4h1-{Guid.NewGuid():N}.db");

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
        private readonly IRegisterWriteTransport _transport;

        public FakeFactory(IRegisterWriteTransport transport)
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
        public FakeConnection(string busId, IRegisterWriteTransport transport)
        {
            BusId = busId;
            RegisterTransport = new NullReadTransport();
            RegisterWriteTransport = transport;
        }

        public string BusId { get; }
        public IRegisterTransport RegisterTransport { get; }
        public IRegisterWriteTransport RegisterWriteTransport { get; }
        public ValueTask DisposeAsync() => ValueTask.CompletedTask;
    }

    private sealed class RecordingTransport : IRegisterWriteTransport
    {
        private readonly ModbusTransportFailureKind? _failureKind;

        public RecordingTransport(ModbusTransportFailureKind? failureKind = null)
        {
            _failureKind = failureKind;
        }

        public int Attempts { get; private set; }
        public List<(string BusId, byte Address, ushort StartAddress, ushort[] Values)> Writes { get; } = [];

        public ValueTask WriteRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            IReadOnlyList<ushort> values,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            Attempts++;

            if (_failureKind is { } kind)
            {
                return ValueTask.FromException(new ModbusTransportFailureException(
                    kind,
                    "simulated B6 physical failure",
                    kind == ModbusTransportFailureKind.Timeout
                        ? new TimeoutException("timeout")
                        : new IOException("io")));
            }

            Writes.Add((busId, unitAddress, startAddress, values.ToArray()));
            return ValueTask.CompletedTask;
        }
    }

    private sealed class NullReadTransport : IRegisterTransport
    {
        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromResult(new ushort[registerCount]);
    }
}
