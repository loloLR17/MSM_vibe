using TR2.Application;
using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class PhysicalPollingWorkRunnerTests
{
    [Fact]
    public async Task StaticPollingReadsB0FromConnectedBusAndMarksCompatibleSession()
    {
        var databasePath = NewDatabasePath();
        try
        {
            const ushort supportedProtocolVersion = 7;
            var registers = new ushort[21];
            registers[1] = 42;
            registers[6] = supportedProtocolVersion;

            var transport = new ScriptedRegisterTransport(registers);
            var composition = Compose(databasePath, new FakeFactory(transport));
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            var now = new DateTimeOffset(2026, 9, 9, 16, 30, 0, TimeSpan.Zero);
            composition.BusWorkScheduler.QueuePolling(endpoint, PollingGroup.Static, now);
            var work = composition.BusWorkScheduler.BeginNext(endpoint.Bus, now)!;

            var runner = new PhysicalPollingWorkRunner(composition, supportedProtocolVersion);
            var result = await runner.ExecuteAsync(work, now);

            Assert.True(result.EndpointCompatible);
            var session = composition.FleetRegistry.GetSession(endpoint);
            Assert.Equal(TR2SessionState.Compatible, session.State);
            Assert.Equal(new DeviceId(42), session.Device!.DeviceId);
            Assert.Equal((endpoint.Bus.Id, endpoint.Address.Value, (ushort)0, (ushort)21), transport.Reads.Single());
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task StaticPollingKeepsProtocolMismatchAsIncompatible()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var registers = new ushort[21];
            registers[1] = 42;
            registers[6] = 8;

            var composition = Compose(databasePath, new FakeFactory(new ScriptedRegisterTransport(registers)));
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            var now = new DateTimeOffset(2026, 9, 9, 16, 31, 0, TimeSpan.Zero);
            composition.BusWorkScheduler.QueuePolling(endpoint, PollingGroup.Static, now);
            var work = composition.BusWorkScheduler.BeginNext(endpoint.Bus, now)!;

            var runner = new PhysicalPollingWorkRunner(composition, supportedProtocolVersion: 7);
            var result = await runner.ExecuteAsync(work, now);

            Assert.False(result.EndpointCompatible);
            Assert.Equal(TR2SessionState.Incompatible, composition.FleetRegistry.GetSession(endpoint).State);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task MissingPhysicalConnectionReturnsNotCompatibleAndReleasesBusSlot()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var composition = Compose(databasePath, new FakeFactory(new ScriptedRegisterTransport(new ushort[21])));
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            await composition.BusConnectionManager.CloseAsync(endpoint.Bus.Id);

            var now = new DateTimeOffset(2026, 9, 9, 16, 32, 0, TimeSpan.Zero);
            composition.BusWorkScheduler.QueuePolling(endpoint, PollingGroup.Static, now);
            var first = composition.BusWorkScheduler.BeginNext(endpoint.Bus, now)!;

            var runner = new PhysicalPollingWorkRunner(composition, supportedProtocolVersion: 7);
            var result = await runner.ExecuteAsync(first, now);

            Assert.False(result.EndpointCompatible);

            composition.BusWorkScheduler.QueuePolling(endpoint, PollingGroup.Static, now);
            Assert.NotNull(composition.BusWorkScheduler.BeginNext(endpoint.Bus, now));
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
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s4f1-{Guid.NewGuid():N}.db");

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

    private sealed class ScriptedRegisterTransport : IRegisterTransport
    {
        private readonly ushort[] _registers;

        public ScriptedRegisterTransport(ushort[] registers)
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
