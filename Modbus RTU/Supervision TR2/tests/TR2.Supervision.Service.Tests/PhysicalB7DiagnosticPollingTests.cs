using TR2.Application;
using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class PhysicalB7DiagnosticPollingTests
{
    [Fact]
    public async Task Medium_polling_publishes_B7_into_authoritative_telemetry_snapshot()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var registers = new ushort[16];
            registers[0] = 1;
            registers[1] = 2;
            registers[2] = 0x0105;
            registers[3] = 17;
            registers[4] = 0x0001;
            registers[5] = 0xE240;
            registers[6] = 3;
            registers[7] = 9;
            registers[8] = 10;
            registers[9] = 0x0009;
            registers[10] = 0xFBF1;
            registers[11] = 2;
            registers[12] = 251;
            registers[13] = 3300;

            var transport = new ScriptedRegisterTransport(registers);
            var composition = Compose(databasePath, new FakeFactory(transport));
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            var deviceId = new DeviceId(42);
            composition.FleetRegistry.SetSession(
                TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));

            var observedAt = new DateTimeOffset(2026, 9, 9, 21, 50, 0, TimeSpan.Zero);
            composition.BusWorkScheduler.QueuePolling(endpoint, PollingGroup.Medium, observedAt);
            var work = composition.BusWorkScheduler.BeginNext(endpoint.Bus, observedAt)!;

            var runner = new PhysicalPollingWorkRunner(composition, supportedProtocolVersion: 1);
            var result = await runner.ExecuteAsync(work, observedAt);

            Assert.True(result.EndpointCompatible);
            Assert.Collection(
                transport.Reads,
                read => Assert.Equal((endpoint.Bus.Id, endpoint.Address.Value, (ushort)2000, (ushort)16), read),
                read => Assert.Equal((endpoint.Bus.Id, endpoint.Address.Value, (ushort)7000, (ushort)16), read));

            var diagnostic = composition.TelemetrySnapshotRegistry.Get(deviceId).DiagnosticState;
            Assert.True(diagnostic.HasValue);
            Assert.True(diagnostic.IsAvailable);
            Assert.Equal(observedAt, diagnostic.ReceivedAt);
            Assert.Equal((ushort)2, diagnostic.LastValue!.SystemHealthStatus);
            Assert.Equal((ushort)17, diagnostic.LastValue.LastFaultCode);
            Assert.Equal((uint)123456, diagnostic.LastValue.LastFaultTimestampSeconds);
            Assert.Equal((ushort)3300, diagnostic.LastValue.SupplyVoltageMillivolts);
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
                  "endpoints": [4]
                }
              ]
            }
            """,
            Path.GetDirectoryName(databasePath)!);

        return SupervisionRuntimeCompositionRoot.Compose(configuration, factory);
    }

    private static string NewDatabasePath() =>
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s7e1-{Guid.NewGuid():N}.db");

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
