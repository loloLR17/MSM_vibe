using TR2.Application;
using TR2.Domain;
using TR2.Protocol;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class PhysicalPriorityWorkRunnerTests
{
    [Fact]
    public async Task ExplicitB1RefreshReadsPhysicalTransportAndUpdatesSnapshot()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var registers = new ushort[B1SystemState.RegisterCount];
            registers[0] = 11;
            registers[4] = 0x0001;
            registers[5] = 0x0002;

            var transport = new ScriptedTransport(registers);
            var composition = Compose(databasePath, new FakeFactory(transport));
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            var deviceId = new DeviceId(42);
            composition.FleetRegistry.SetSession(
                TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));

            var operations = new SupervisionOperationalFacade(composition);
            var now = new DateTimeOffset(2026, 9, 9, 17, 0, 0, TimeSpan.Zero);
            var refresh = operations
                .QueuePostReconnectRefresh(endpoint, now)
                .Single(candidate => candidate.Block == TR2RegisterBlock.B1);

            var work = TakeUntil(composition.BusWorkScheduler, endpoint.Bus, now, refresh.Work.WorkId);
            var runner = new PhysicalPriorityWorkRunner(composition, operations);
            await runner.ExecuteAsync(work, now);

            var snapshot = composition.TelemetrySnapshotRegistry.Get(deviceId).SystemState;
            Assert.True(snapshot.HasValue);
            Assert.True(snapshot.IsAvailable);
            Assert.Equal(now, snapshot.ReceivedAt);
            Assert.Equal((ushort)11, snapshot.LastValue!.SystemStatus);
            Assert.Equal((uint)0x00010002, snapshot.LastValue.UptimeSeconds);
            Assert.Contains(
                (endpoint.Bus.Id, endpoint.Address.Value, B1SystemState.StartAddress, (ushort)B1SystemState.RegisterCount),
                transport.Reads);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task ExplicitRefreshTimeoutKeepsPhysicalConnectionAndSession()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var transport = new FailingTransport(ModbusTransportFailureKind.Timeout);
            var composition = Compose(databasePath, new FakeFactory(transport));
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            var deviceId = new DeviceId(43);
            composition.FleetRegistry.SetSession(
                TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));

            var operations = new SupervisionOperationalFacade(composition);
            var now = new DateTimeOffset(2026, 9, 9, 17, 1, 0, TimeSpan.Zero);
            var refresh = operations
                .QueuePostReconnectRefresh(endpoint, now)
                .Single(candidate => candidate.Block == TR2RegisterBlock.B1);

            var work = TakeUntil(composition.BusWorkScheduler, endpoint.Bus, now, refresh.Work.WorkId);
            await new PhysicalPriorityWorkRunner(composition, operations).ExecuteAsync(work, now);

            Assert.True(composition.BusConnectionManager.TryGet(endpoint.Bus.Id, out _));
            Assert.Equal(TR2SessionState.Compatible, composition.FleetRegistry.GetSession(endpoint).State);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task ExplicitRefreshIoFailureClosesBusAndInvalidatesSession()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var transport = new FailingTransport(ModbusTransportFailureKind.Io);
            var composition = Compose(databasePath, new FakeFactory(transport));
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            composition.FleetRegistry.SetSession(
                TR2Session.CreateCompatible(endpoint, new TR2Device(new DeviceId(44))));

            var operations = new SupervisionOperationalFacade(composition);
            var now = new DateTimeOffset(2026, 9, 9, 17, 2, 0, TimeSpan.Zero);
            var refresh = operations
                .QueuePostReconnectRefresh(endpoint, now)
                .Single(candidate => candidate.Block == TR2RegisterBlock.B1);

            var work = TakeUntil(composition.BusWorkScheduler, endpoint.Bus, now, refresh.Work.WorkId);
            await new PhysicalPriorityWorkRunner(composition, operations).ExecuteAsync(work, now);

            Assert.False(composition.BusConnectionManager.TryGet(endpoint.Bus.Id, out _));
            Assert.Equal(TR2SessionState.Unidentified, composition.FleetRegistry.GetSession(endpoint).State);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    private static ScheduledBusWork TakeUntil(
        BusWorkScheduler scheduler,
        SerialBus bus,
        DateTimeOffset now,
        long workId)
    {
        while (true)
        {
            var work = scheduler.BeginNext(bus, now)
                ?? throw new InvalidOperationException("Expected queued priority work.");

            if (work.WorkId == workId)
            {
                return work;
            }

            scheduler.Complete(bus, work.WorkId);
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
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s4g1-{Guid.NewGuid():N}.db");

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
        private readonly IRegisterTransport _transport;

        public FakeFactory(IRegisterTransport transport)
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
        public FakeConnection(string busId, IRegisterTransport transport)
        {
            BusId = busId;
            RegisterTransport = transport;
            RegisterWriteTransport = new NullWriteTransport();
        }

        public string BusId { get; }
        public IRegisterTransport RegisterTransport { get; }
        public IRegisterWriteTransport RegisterWriteTransport { get; }

        public ValueTask DisposeAsync() => ValueTask.CompletedTask;
    }

    private sealed class ScriptedTransport : IRegisterTransport
    {
        private readonly ushort[] _registers;

        public ScriptedTransport(ushort[] registers)
        {
            _registers = registers;
        }

        public List<(string BusId, byte Address, ushort StartAddress, ushort RegisterCount)> Reads { get; } = [];

        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            Reads.Add((busId, unitAddress, startAddress, registerCount));
            return ValueTask.FromResult(_registers.Take(registerCount).ToArray());
        }
    }

    private sealed class FailingTransport : IRegisterTransport
    {
        private readonly ModbusTransportFailureKind _kind;

        public FailingTransport(ModbusTransportFailureKind kind)
        {
            _kind = kind;
        }

        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromException<ushort[]>(new ModbusTransportFailureException(
                _kind,
                "simulated physical failure",
                _kind == ModbusTransportFailureKind.Timeout
                    ? new TimeoutException("timeout")
                    : new IOException("io")));
    }

    private sealed class NullWriteTransport : IRegisterWriteTransport
    {
        public ValueTask WriteRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            IReadOnlyList<ushort> values,
            CancellationToken cancellationToken = default) =>
            ValueTask.CompletedTask;
    }
}
