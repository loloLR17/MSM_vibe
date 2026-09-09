using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class SupervisionRuntimeStartupRollbackTests
{
    [Fact]
    public async Task StartupDisposesPreviouslyOpenedBusesWhenLaterSerialBusOpenFails()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var configuration = RuntimeConfigurationLoader.Parse(
                $$"""
                {
                  "persistence": { "databasePath": "{{databasePath.Replace("\\", "\\\\")}}" },
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
                    },
                    {
                      "id": "bus-2",
                      "serial": {
                        "portName": "COM8",
                        "baudRate": 115200,
                        "dataBits": 8,
                        "parity": "None",
                        "stopBits": "One",
                        "responseTimeoutMilliseconds": 750
                      },
                      "endpoints": [2]
                    }
                  ]
                }
                """,
                Path.GetDirectoryName(databasePath)!);
            var factory = new FailSecondConnectionFactory();
            var composition = SupervisionRuntimeCompositionRoot.Compose(configuration, factory);

            await Assert.ThrowsAsync<IOException>(async () =>
                await new SupervisionRuntimeStartup(composition).StartAsync());

            var first = Assert.Single(factory.OpenedConnections);
            Assert.True(first.Disposed);
            Assert.False(composition.ReadinessGate.IsReady);
            Assert.Throws<ObjectDisposedException>(() =>
                composition.BusConnectionManager.TryGet("bus-1", out _));
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    private static string NewDatabasePath() =>
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s5a-{Guid.NewGuid():N}.db");

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

    private sealed class FailSecondConnectionFactory : IModbusBusConnectionFactory
    {
        private int _openCount;
        public List<FakeConnection> OpenedConnections { get; } = [];

        public ValueTask<IModbusBusConnection> OpenAsync(
            string busId,
            ModbusSerialConnectionSettings settings,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            _openCount++;

            if (_openCount == 2)
            {
                return ValueTask.FromException<IModbusBusConnection>(new IOException("second port unavailable"));
            }

            var connection = new FakeConnection(busId);
            OpenedConnections.Add(connection);
            return ValueTask.FromResult<IModbusBusConnection>(connection);
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
