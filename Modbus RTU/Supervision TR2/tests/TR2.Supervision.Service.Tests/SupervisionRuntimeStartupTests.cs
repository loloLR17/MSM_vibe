using TR2.Application;
using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class SupervisionRuntimeStartupTests
{
    [Fact]
    public async Task StartupCreatesDatabaseAndOpensReadinessGate()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var composition = Compose(databasePath);

            Assert.False(composition.ReadinessGate.IsReady);
            Assert.Throws<InvalidOperationException>(() => composition.ReadinessGate.EnsureReady());

            var startup = new SupervisionRuntimeStartup(composition);
            await startup.StartAsync();

            Assert.True(composition.ReadinessGate.IsReady);
            composition.ReadinessGate.EnsureReady();
            Assert.True(File.Exists(databasePath));
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task StartupOpensOnlyConfiguredSerialBusesWithExactSettings()
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
                        "parity": "Even",
                        "stopBits": "Two",
                        "responseTimeoutMilliseconds": 750
                      },
                      "endpoints": [1]
                    },
                    {
                      "id": "bus-logical-only",
                      "endpoints": [2]
                    }
                  ]
                }
                """,
                Path.GetDirectoryName(databasePath)!);
            var factory = new RecordingConnectionFactory();
            var composition = SupervisionRuntimeCompositionRoot.Compose(configuration, factory);

            await new SupervisionRuntimeStartup(composition).StartAsync();

            var request = Assert.Single(factory.Requests);
            Assert.Equal("bus-serial", request.BusId);
            Assert.Equal("COM7", request.Settings.PortName);
            Assert.Equal(115200, request.Settings.BaudRate);
            Assert.Equal(8, request.Settings.DataBits);
            Assert.Equal(ModbusSerialParity.Even, request.Settings.Parity);
            Assert.Equal(ModbusSerialStopBits.Two, request.Settings.StopBits);
            Assert.Equal(750, request.Settings.ResponseTimeoutMilliseconds);
            Assert.True(composition.BusConnectionManager.TryGet("bus-serial", out _));
            Assert.False(composition.BusConnectionManager.TryGet("bus-logical-only", out _));
            Assert.True(composition.ReadinessGate.IsReady);

            await composition.BusConnectionManager.DisposeAsync();
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task StartupDoesNotOpenReadinessGateWhenSerialBusOpenFails()
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
            var factory = new RecordingConnectionFactory
            {
                Failure = new IOException("port unavailable")
            };
            var composition = SupervisionRuntimeCompositionRoot.Compose(configuration, factory);

            await Assert.ThrowsAsync<IOException>(async () =>
                await new SupervisionRuntimeStartup(composition).StartAsync());

            Assert.False(composition.ReadinessGate.IsReady);
            Assert.Empty(composition.BusConnectionManager.ConnectedBusIds);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task StartupRestoresNonterminalCommandAsAmbiguousBeforeReady()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var first = Compose(databasePath);
            await new SupervisionRuntimeStartup(first).StartAsync();

            var deviceId = new DeviceId(42);
            var coordinator = new CommandCoordinator(
                deviceId,
                first.CommandReservationStore,
                first.CommandJournal);

            var prepared = await coordinator.PrepareAsync(
                "request-42",
                new DateTimeOffset(2026, 9, 9, 12, 0, 0, TimeSpan.Zero));
            await coordinator.MarkSubmittedAsync(
                new DateTimeOffset(2026, 9, 9, 12, 0, 1, TimeSpan.Zero));

            var second = Compose(databasePath);
            Assert.False(second.ReadinessGate.IsReady);

            await new SupervisionRuntimeStartup(second).StartAsync();

            Assert.True(second.ReadinessGate.IsReady);
            var recovered = second.CommandCoordinatorRegistry.Get(deviceId).ActiveTransaction;
            Assert.NotNull(recovered);
            Assert.Equal(prepared.TransactionId, recovered.TransactionId);
            Assert.Equal("request-42", recovered.RequestIdentity);
            Assert.Equal(CommandTransactionState.Ambiguous, recovered.State);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task StartupRegistersJournalKnownDeviceWithNoActiveTransactionWhenHistoryIsTerminal()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var first = Compose(databasePath);
            await new SupervisionRuntimeStartup(first).StartAsync();

            var deviceId = new DeviceId(77);
            var coordinator = new CommandCoordinator(
                deviceId,
                first.CommandReservationStore,
                first.CommandJournal);

            var prepared = await coordinator.PrepareAsync(
                "request-77",
                new DateTimeOffset(2026, 9, 9, 13, 0, 0, TimeSpan.Zero));
            await coordinator.MarkSubmittedAsync(
                new DateTimeOffset(2026, 9, 9, 13, 0, 1, TimeSpan.Zero));
            await coordinator.ResolveTerminalAsync(
                prepared.TransactionId,
                new DateTimeOffset(2026, 9, 9, 13, 0, 2, TimeSpan.Zero));

            var second = Compose(databasePath);
            await new SupervisionRuntimeStartup(second).StartAsync();

            var recoveredCoordinator = second.CommandCoordinatorRegistry.Get(deviceId);
            Assert.Null(recoveredCoordinator.ActiveTransaction);
            Assert.True(second.ReadinessGate.IsReady);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task StartupIsOneShot()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var composition = Compose(databasePath);
            var startup = new SupervisionRuntimeStartup(composition);

            await startup.StartAsync();

            await Assert.ThrowsAsync<InvalidOperationException>(async () => await startup.StartAsync());
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    private static SupervisionRuntimeComposition Compose(string databasePath)
    {
        var configuration = RuntimeConfigurationLoader.Parse(
            $$"""
            {
              "persistence": { "databasePath": "{{databasePath.Replace("\\", "\\\\")}}" },
              "buses": [
                { "id": "bus-1", "endpoints": [1] }
              ]
            }
            """,
            Path.GetDirectoryName(databasePath)!);

        return SupervisionRuntimeCompositionRoot.Compose(configuration);
    }

    private static string NewDatabasePath() =>
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s3c-{Guid.NewGuid():N}.db");

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
        public List<(string BusId, ModbusSerialConnectionSettings Settings)> Requests { get; } = [];
        public Exception? Failure { get; init; }

        public ValueTask<IModbusBusConnection> OpenAsync(
            string busId,
            ModbusSerialConnectionSettings settings,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            Requests.Add((busId, settings));

            if (Failure is not null)
            {
                return ValueTask.FromException<IModbusBusConnection>(Failure);
            }

            return ValueTask.FromResult<IModbusBusConnection>(new FakeConnection(busId));
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

        public ValueTask DisposeAsync() => ValueTask.CompletedTask;
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
