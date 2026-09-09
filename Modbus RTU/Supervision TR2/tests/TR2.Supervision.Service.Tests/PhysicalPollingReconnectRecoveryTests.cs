using TR2.Application;
using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class PhysicalPollingReconnectRecoveryTests
{
    [Fact]
    public async Task IoFailureDisconnectsThenReconnectRequiresFreshB0BeforeCompatible()
    {
        var databasePath = NewDatabasePath();
        try
        {
            const ushort supportedProtocolVersion = 7;
            var firstTransport = new IoAfterB0Transport(supportedProtocolVersion, deviceId: 42);
            var secondTransport = new B0OnlyTransport(supportedProtocolVersion, deviceId: 42);
            var factory = new SequencedFactory(firstTransport, secondTransport);
            var composition = Compose(databasePath, factory);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            var runner = new PhysicalPollingWorkRunner(composition, supportedProtocolVersion);
            var recoveryScheduler = new SerialBusReconnectScheduler(
                composition,
                TimeSpan.FromSeconds(5));
            var t0 = new DateTimeOffset(2026, 9, 9, 17, 0, 0, TimeSpan.Zero);

            var firstDiscovery = BeginPolling(
                composition,
                endpoint,
                PollingGroup.Static,
                t0);
            var discoveryResult = await runner.ExecuteAsync(firstDiscovery, t0);

            Assert.True(discoveryResult.EndpointCompatible);
            Assert.Equal(TR2SessionState.Compatible, composition.FleetRegistry.GetSession(endpoint).State);
            Assert.True(composition.BusConnectionManager.TryGet(endpoint.Bus.Id, out _));

            var failedFastPolling = BeginPolling(
                composition,
                endpoint,
                PollingGroup.Fast,
                t0.AddSeconds(1));
            var failedResult = await runner.ExecuteAsync(failedFastPolling, t0.AddSeconds(1));

            Assert.False(failedResult.EndpointCompatible);
            Assert.Equal(TR2SessionState.Unidentified, composition.FleetRegistry.GetSession(endpoint).State);
            Assert.False(composition.BusConnectionManager.TryGet(endpoint.Bus.Id, out _));
            Assert.Single(factory.OpenedTransports);

            await recoveryScheduler.RunDueAsync(t0.AddSeconds(1));
            await recoveryScheduler.RunDueAsync(t0.AddSeconds(5));

            Assert.Single(factory.OpenedTransports);
            Assert.False(composition.BusConnectionManager.TryGet(endpoint.Bus.Id, out _));

            await recoveryScheduler.RunDueAsync(t0.AddSeconds(6));

            Assert.Equal(2, factory.OpenedTransports.Count);
            Assert.True(composition.BusConnectionManager.TryGet(endpoint.Bus.Id, out _));
            Assert.Equal(TR2SessionState.Unidentified, composition.FleetRegistry.GetSession(endpoint).State);

            var secondDiscovery = BeginPolling(
                composition,
                endpoint,
                PollingGroup.Static,
                t0.AddSeconds(6));
            var rediscoveryResult = await runner.ExecuteAsync(secondDiscovery, t0.AddSeconds(6));

            Assert.True(rediscoveryResult.EndpointCompatible);
            var recoveredSession = composition.FleetRegistry.GetSession(endpoint);
            Assert.Equal(TR2SessionState.Compatible, recoveredSession.State);
            Assert.Equal(new DeviceId(42), recoveredSession.Device!.DeviceId);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    private static ScheduledBusWork BeginPolling(
        SupervisionRuntimeComposition composition,
        TR2Endpoint endpoint,
        PollingGroup group,
        DateTimeOffset dueAt)
    {
        composition.BusWorkScheduler.QueuePolling(endpoint, group, dueAt);
        return composition.BusWorkScheduler.BeginNext(endpoint.Bus, dueAt)
            ?? throw new InvalidOperationException("Expected polling work to be runnable.");
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
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s4f3b-{Guid.NewGuid():N}.db");

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

    private sealed class SequencedFactory : IModbusBusConnectionFactory
    {
        private readonly Queue<IRegisterTransport> _transports;

        public SequencedFactory(params IRegisterTransport[] transports)
        {
            _transports = new Queue<IRegisterTransport>(transports);
        }

        public List<IRegisterTransport> OpenedTransports { get; } = [];

        public ValueTask<IModbusBusConnection> OpenAsync(
            string busId,
            ModbusSerialConnectionSettings settings,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();

            if (_transports.Count == 0)
            {
                return ValueTask.FromException<IModbusBusConnection>(
                    new InvalidOperationException("No scripted connection remains."));
            }

            var transport = _transports.Dequeue();
            OpenedTransports.Add(transport);
            return ValueTask.FromResult<IModbusBusConnection>(
                new FakeConnection(busId, transport));
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

    private sealed class IoAfterB0Transport : IRegisterTransport
    {
        private readonly ushort _protocolVersion;
        private readonly uint _deviceId;

        public IoAfterB0Transport(ushort protocolVersion, uint deviceId)
        {
            _protocolVersion = protocolVersion;
            _deviceId = deviceId;
        }

        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();

            if (startAddress == 0)
            {
                return ValueTask.FromResult(CreateB0(_protocolVersion, _deviceId));
            }

            return ValueTask.FromException<ushort[]>(
                new ModbusTransportFailureException(
                    ModbusTransportFailureKind.Io,
                    "Simulated serial adapter removal.",
                    new IOException("USB-RS485 removed")));
        }
    }

    private sealed class B0OnlyTransport : IRegisterTransport
    {
        private readonly ushort _protocolVersion;
        private readonly uint _deviceId;

        public B0OnlyTransport(ushort protocolVersion, uint deviceId)
        {
            _protocolVersion = protocolVersion;
            _deviceId = deviceId;
        }

        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            return ValueTask.FromResult(CreateB0(_protocolVersion, _deviceId));
        }
    }

    private static ushort[] CreateB0(ushort protocolVersion, uint deviceId)
    {
        var registers = new ushort[21];
        registers[0] = (ushort)(deviceId >> 16);
        registers[1] = (ushort)deviceId;
        registers[6] = protocolVersion;
        return registers;
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
