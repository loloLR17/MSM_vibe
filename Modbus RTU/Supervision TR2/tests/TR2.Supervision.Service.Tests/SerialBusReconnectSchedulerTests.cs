using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class SerialBusReconnectSchedulerTests
{
    [Fact]
    public void ConstructorRejectsNonPositiveReconnectInterval()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var composition = Compose(databasePath, new FakeFactory());

            Assert.Throws<ArgumentOutOfRangeException>(() =>
                new SerialBusReconnectScheduler(composition, TimeSpan.Zero));
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task MissingConnectionIsScheduledBeforeAnyReconnectAttempt()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var factory = new FakeFactory();
            var composition = Compose(databasePath, factory);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var bus = composition.Configuration.Buses.Single().Bus;
            await new SerialBusRecoveryCoordinator(composition).MarkDisconnectedAsync(bus);

            var scheduler = new SerialBusReconnectScheduler(composition, TimeSpan.FromSeconds(5));
            var observedAt = new DateTimeOffset(2026, 9, 9, 12, 0, 0, TimeSpan.Zero);

            await scheduler.RunDueAsync(observedAt);

            Assert.Single(factory.OpenRequests);
            Assert.True(scheduler.TryGetNextAttempt(bus, out var dueAt));
            Assert.Equal(observedAt + TimeSpan.FromSeconds(5), dueAt);
            Assert.False(composition.BusConnectionManager.TryGet(bus.Id, out _));
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task ReconnectIsNotAttemptedBeforeDueTimeAndRunsAtDueTime()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var factory = new FakeFactory();
            var composition = Compose(databasePath, factory);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var bus = composition.Configuration.Buses.Single().Bus;
            await new SerialBusRecoveryCoordinator(composition).MarkDisconnectedAsync(bus);

            var scheduler = new SerialBusReconnectScheduler(composition, TimeSpan.FromSeconds(5));
            var observedAt = new DateTimeOffset(2026, 9, 9, 12, 0, 0, TimeSpan.Zero);

            await scheduler.RunDueAsync(observedAt);
            await scheduler.RunDueAsync(observedAt + TimeSpan.FromSeconds(4));
            Assert.Single(factory.OpenRequests);

            await scheduler.RunDueAsync(observedAt + TimeSpan.FromSeconds(5));

            Assert.Equal(2, factory.OpenRequests.Count);
            Assert.False(scheduler.TryGetNextAttempt(bus, out _));
            Assert.True(composition.BusConnectionManager.TryGet(bus.Id, out _));
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task FailedReconnectIsContainedAndRescheduled()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var factory = new FakeFactory();
            var composition = Compose(databasePath, factory);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var bus = composition.Configuration.Buses.Single().Bus;
            await new SerialBusRecoveryCoordinator(composition).MarkDisconnectedAsync(bus);
            factory.NextFailure = new IOException("port still unavailable");

            var scheduler = new SerialBusReconnectScheduler(composition, TimeSpan.FromSeconds(5));
            var observedAt = new DateTimeOffset(2026, 9, 9, 12, 0, 0, TimeSpan.Zero);

            await scheduler.RunDueAsync(observedAt);
            await scheduler.RunDueAsync(observedAt + TimeSpan.FromSeconds(5));

            Assert.Equal(2, factory.OpenRequests.Count);
            Assert.True(scheduler.TryGetNextAttempt(bus, out var nextAttempt));
            Assert.Equal(observedAt + TimeSpan.FromSeconds(10), nextAttempt);
            Assert.False(composition.BusConnectionManager.TryGet(bus.Id, out _));
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task ConnectedBusDoesNotAcquireReconnectSchedule()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var factory = new FakeFactory();
            var composition = Compose(databasePath, factory);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var bus = composition.Configuration.Buses.Single().Bus;
            var scheduler = new SerialBusReconnectScheduler(composition, TimeSpan.FromSeconds(5));

            await scheduler.RunDueAsync(new DateTimeOffset(2026, 9, 9, 12, 0, 0, TimeSpan.Zero));

            Assert.Single(factory.OpenRequests);
            Assert.False(scheduler.TryGetNextAttempt(bus, out _));
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
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s4e2a-{Guid.NewGuid():N}.db");

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
        public Exception? NextFailure { get; set; }

        public ValueTask<IModbusBusConnection> OpenAsync(
            string busId,
            ModbusSerialConnectionSettings settings,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            OpenRequests.Add((busId, settings));

            if (NextFailure is not null)
            {
                var failure = NextFailure;
                NextFailure = null;
                return ValueTask.FromException<IModbusBusConnection>(failure);
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
