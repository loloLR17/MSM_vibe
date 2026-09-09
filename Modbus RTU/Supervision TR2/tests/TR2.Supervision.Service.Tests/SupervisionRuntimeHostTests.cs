using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class SupervisionRuntimeHostTests
{
    [Fact]
    public async Task RunAsyncOpensReadinessThenClosesItOnCancellation()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var composition = Compose(databasePath);
            var host = new SupervisionRuntimeHost(composition);
            using var cancellation = new CancellationTokenSource();

            var runTask = host.RunAsync(cancellation.Token);
            await WaitForStateAsync(host, SupervisionRuntimeState.Running);

            Assert.True(composition.ReadinessGate.IsReady);
            composition.ReadinessGate.EnsureReady();

            cancellation.Cancel();
            await runTask;

            Assert.Equal(SupervisionRuntimeState.Stopped, host.State);
            Assert.False(composition.ReadinessGate.IsReady);
            Assert.Throws<InvalidOperationException>(() => composition.ReadinessGate.EnsureReady());
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task RunAsyncDisposesConfiguredSerialConnectionOnShutdown()
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
                      "id": "bus-serial",
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
            var factory = new RecordingConnectionFactory();
            var composition = SupervisionRuntimeCompositionRoot.Compose(configuration, factory);
            var host = new SupervisionRuntimeHost(composition);
            using var cancellation = new CancellationTokenSource();

            var runTask = host.RunAsync(cancellation.Token);
            await WaitForStateAsync(host, SupervisionRuntimeState.Running);
            var connection = Assert.Single(factory.Connections);
            Assert.False(connection.Disposed);

            cancellation.Cancel();
            await runTask;

            Assert.True(connection.Disposed);
            Assert.Equal(SupervisionRuntimeState.Stopped, host.State);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task RunAsyncWithAlreadyCancelledTokenStopsWithoutBecomingReady()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var composition = Compose(databasePath);
            var host = new SupervisionRuntimeHost(composition);
            using var cancellation = new CancellationTokenSource();
            cancellation.Cancel();

            await host.RunAsync(cancellation.Token);

            Assert.Equal(SupervisionRuntimeState.Stopped, host.State);
            Assert.False(composition.ReadinessGate.IsReady);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task RuntimeHostIsOneShot()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var composition = Compose(databasePath);
            var host = new SupervisionRuntimeHost(composition);
            using var cancellation = new CancellationTokenSource();

            var runTask = host.RunAsync(cancellation.Token);
            await WaitForStateAsync(host, SupervisionRuntimeState.Running);
            cancellation.Cancel();
            await runTask;

            await Assert.ThrowsAsync<InvalidOperationException>(async () => await host.RunAsync());
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    private static async Task WaitForStateAsync(
        SupervisionRuntimeHost host,
        SupervisionRuntimeState expected)
    {
        using var timeout = new CancellationTokenSource(TimeSpan.FromSeconds(5));
        while (host.State != expected)
        {
            if (host.State is SupervisionRuntimeState.Faulted or SupervisionRuntimeState.Stopped)
            {
                throw new InvalidOperationException($"Runtime reached unexpected state {host.State} while waiting for {expected}.");
            }

            await Task.Delay(10, timeout.Token);
        }
    }

    private static SupervisionRuntimeComposition Compose(string databasePath)
    {
        var configuration = RuntimeConfigurationLoader.Parse(
            $$"""
            {
              "persistence": { "databasePath": "{{databasePath.Replace("\\", "\\\\")}}" },
              "buses": []
            }
            """,
            Path.GetDirectoryName(databasePath)!);

        return SupervisionRuntimeCompositionRoot.Compose(configuration);
    }

    private static string NewDatabasePath() =>
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s3d-{Guid.NewGuid():N}.db");

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

    private sealed class RecordingConnectionFactory : IModbusBusConnectionFactory
    {
        public List<FakeConnection> Connections { get; } = [];

        public ValueTask<IModbusBusConnection> OpenAsync(
            string busId,
            ModbusSerialConnectionSettings settings,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            var connection = new FakeConnection(busId);
            Connections.Add(connection);
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
