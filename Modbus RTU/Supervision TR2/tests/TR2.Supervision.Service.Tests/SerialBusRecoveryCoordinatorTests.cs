using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class SerialBusRecoveryCoordinatorTests
{
    [Fact]
    public async Task DisconnectClosesPhysicalConnectionAndResetsSessionsToUnidentified()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var factory = new FakeFactory();
            var composition = Compose(databasePath, factory, includeSerial: true);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            composition.FleetRegistry.SetSession(
                TR2Session.CreateCompatible(endpoint, new TR2Device(new DeviceId(42))));

            var connection = Assert.IsType<FakeConnection>(factory.Connections.Single());
            var recovery = new SerialBusRecoveryCoordinator(composition);

            await recovery.MarkDisconnectedAsync(endpoint.Bus);

            Assert.True(connection.Disposed);
            Assert.False(composition.BusConnectionManager.TryGet(endpoint.Bus.Id, out _));
            Assert.Equal(TR2SessionState.Unidentified, composition.FleetRegistry.GetSession(endpoint).State);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task ReconnectReopensConfiguredSerialBusAndRequiresFreshIdentification()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var factory = new FakeFactory();
            var composition = Compose(databasePath, factory, includeSerial: true);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            var recovery = new SerialBusRecoveryCoordinator(composition);
            await recovery.MarkDisconnectedAsync(endpoint.Bus);

            var reconnected = await recovery.TryReconnectAsync(endpoint.Bus);

            Assert.True(reconnected);
            Assert.Equal(2, factory.OpenRequests.Count);
            Assert.True(composition.BusConnectionManager.TryGet(endpoint.Bus.Id, out _));
            Assert.Equal(TR2SessionState.Unidentified, composition.FleetRegistry.GetSession(endpoint).State);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task ReconnectReturnsFalseForLogicalBusWithoutSerialConfiguration()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var factory = new FakeFactory();
            var composition = Compose(databasePath, factory, includeSerial: false);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var bus = composition.Configuration.Buses.Single().Bus;
            var recovery = new SerialBusRecoveryCoordinator(composition);

            var reconnected = await recovery.TryReconnectAsync(bus);

            Assert.False(reconnected);
            Assert.Empty(factory.OpenRequests);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    private static SupervisionRuntimeComposition Compose(
        string databasePath,
        IModbusBusConnectionFactory factory,
        bool includeSerial)
    {
        var escapedDatabasePath = databasePath.Replace("\\", "\\\\");
        var serial = includeSerial
            ? """
              "serial": {
                "portName": "COM7",
                "baudRate": 115200,
                "dataBits": 8,
                "parity": "None",
                "stopBits": "One",
                "responseTimeoutMilliseconds": 750
              },
              """
            : string.Empty;

        var configuration = RuntimeConfigurationLoader.Parse(
            $$"""
            {
              "persistence": { "databasePath": "{{escapedDatabasePath}}" },
              "buses": [
                {
                  "id": "bus-1",
                  {{serial}}
                  "endpoints": [1]
                }
              ]
            }
            """,
            Path.GetDirectoryName(databasePath)!);

        return SupervisionRuntimeCompositionRoot.Compose(configuration, factory);
    }

    private static string NewDatabasePath() =>
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s4e1-{Guid.NewGuid():N}.db");

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
        public List<(string BusId, ModbusSerialConnectionSettings Settings)> OpenRequests { get; } = [];
        public List<IModbusBusConnection> Connections { get; } = [];

        public ValueTask<IModbusBusConnection> OpenAsync(
            string busId,
            ModbusSerialConnectionSettings settings,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            OpenRequests.Add((busId, settings));
            IModbusBusConnection connection = new FakeConnection(busId);
            Connections.Add(connection);
            return ValueTask.FromResult(connection);
        }
    }

    private sealed class FakeConnection : IModbusBusConnection
    {
        public FakeConnection(string busId)
        {
            BusId = busId;
            var transport = new NullRegisterTransport();
            RegisterTransport = transport;
            RegisterWriteTransport = transport;
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

    private sealed class NullRegisterTransport : IRegisterTransport, IRegisterWriteTransport
    {
        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromResult(Array.Empty<ushort>());

        public ValueTask WriteRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            IReadOnlyList<ushort> values,
            CancellationToken cancellationToken = default) =>
            ValueTask.CompletedTask;
    }
}
