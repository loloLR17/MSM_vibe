using TR2.Application;
using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class PhysicalPollingFailureRecoveryTests
{
    [Fact]
    public async Task TimeoutKeepsPhysicalConnectionAndIdentifiedSession()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var transport = new FailingRegisterTransport(ModbusTransportFailureKind.Timeout);
            var factory = new FakeFactory(transport);
            var composition = Compose(databasePath, factory);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            composition.FleetRegistry.SetSession(
                TR2Session.CreateCompatible(endpoint, new TR2Device(new DeviceId(42))));

            var now = new DateTimeOffset(2026, 9, 9, 16, 40, 0, TimeSpan.Zero);
            composition.BusWorkScheduler.QueuePolling(endpoint, PollingGroup.Fast, now);
            var work = composition.BusWorkScheduler.BeginNext(endpoint.Bus, now)!;

            var runner = new PhysicalPollingWorkRunner(composition, supportedProtocolVersion: 7);
            var result = await runner.ExecuteAsync(work, now);

            Assert.False(result.EndpointCompatible);
            Assert.True(composition.BusConnectionManager.TryGet(endpoint.Bus.Id, out _));
            Assert.Equal(TR2SessionState.Compatible, composition.FleetRegistry.GetSession(endpoint).State);
            Assert.False(factory.Connection!.Disposed);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task IoFailureClosesPhysicalConnectionAndResetsSession()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var transport = new FailingRegisterTransport(ModbusTransportFailureKind.Io);
            var factory = new FakeFactory(transport);
            var composition = Compose(databasePath, factory);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            composition.FleetRegistry.SetSession(
                TR2Session.CreateCompatible(endpoint, new TR2Device(new DeviceId(42))));

            var now = new DateTimeOffset(2026, 9, 9, 16, 41, 0, TimeSpan.Zero);
            composition.BusWorkScheduler.QueuePolling(endpoint, PollingGroup.Fast, now);
            var work = composition.BusWorkScheduler.BeginNext(endpoint.Bus, now)!;

            var runner = new PhysicalPollingWorkRunner(composition, supportedProtocolVersion: 7);
            var result = await runner.ExecuteAsync(work, now);

            Assert.False(result.EndpointCompatible);
            Assert.False(composition.BusConnectionManager.TryGet(endpoint.Bus.Id, out _));
            Assert.Equal(TR2SessionState.Unidentified, composition.FleetRegistry.GetSession(endpoint).State);
            Assert.True(factory.Connection!.Disposed);
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
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s4f3-{Guid.NewGuid():N}.db");

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

        public FakeConnection? Connection { get; private set; }

        public ValueTask<IModbusBusConnection> OpenAsync(
            string busId,
            ModbusSerialConnectionSettings settings,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            Connection = new FakeConnection(busId, _transport);
            return ValueTask.FromResult<IModbusBusConnection>(Connection);
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
        public bool Disposed { get; private set; }

        public ValueTask DisposeAsync()
        {
            Disposed = true;
            return ValueTask.CompletedTask;
        }
    }

    private sealed class FailingRegisterTransport : IRegisterTransport
    {
        private readonly ModbusTransportFailureKind _kind;

        public FailingRegisterTransport(ModbusTransportFailureKind kind)
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
                    ? new TimeoutException()
                    : new IOException()));
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
