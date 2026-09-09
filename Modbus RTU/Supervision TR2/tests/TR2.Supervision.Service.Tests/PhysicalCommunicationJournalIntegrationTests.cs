using TR2.Application;
using TR2.Domain;
using TR2.Protocol;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class PhysicalCommunicationJournalIntegrationTests
{
    [Fact]
    public async Task PollingTimeoutPersistsStableFailureContext()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var transport = new FailingTransport(readFailure: Failure(ModbusTransportFailureKind.Timeout));
            var composition = Compose(databasePath, transport);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = Identify(composition, 101);
            var now = At(0);
            composition.BusWorkScheduler.QueuePolling(endpoint, PollingGroup.Fast, now);
            var work = composition.BusWorkScheduler.BeginNext(endpoint.Bus, now)!;

            await new PhysicalPollingWorkRunner(composition, supportedProtocolVersion: 7)
                .ExecuteAsync(work, now);

            AssertFailure(composition, "Polling", "Timeout", 101);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task ExplicitRefreshIoFailurePersistsStableFailureContext()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var transport = new FailingTransport(readFailure: Failure(ModbusTransportFailureKind.Io));
            var composition = Compose(databasePath, transport);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = Identify(composition, 102);
            var operations = new SupervisionOperationalFacade(composition);
            var now = At(1);
            var refresh = operations.QueuePostReconnectRefresh(endpoint, now)
                .Single(candidate => candidate.Block == TR2RegisterBlock.B1);
            var work = TakeUntil(composition.BusWorkScheduler, endpoint.Bus, now, refresh.Work.WorkId);

            await new PhysicalPriorityWorkRunner(composition, operations)
                .ExecuteAsync(work, now);

            AssertFailure(composition, "ExplicitRefresh", "Io", 102);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task B5TimeoutPersistsStableFailureContext()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var transport = new FailingTransport(writeFailure: Failure(ModbusTransportFailureKind.Timeout));
            var composition = Compose(databasePath, transport);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = Identify(composition, 103);
            var operations = new SupervisionOperationalFacade(composition);
            var now = At(2);
            await operations.QueueCommandAsync(
                endpoint,
                "journal-b5",
                new B5CommandIntent(5, 0, 0, 0, 0),
                now);
            var work = composition.BusWorkScheduler.BeginNext(endpoint.Bus, now)!;

            await new PhysicalPriorityWorkRunner(composition, operations)
                .ExecuteAsync(work, now);

            AssertFailure(composition, "CommandTransaction", "Timeout", 103);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task B6TimeoutPersistsStableFailureContext()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var transport = new FailingTransport(writeFailure: Failure(ModbusTransportFailureKind.Timeout));
            var composition = Compose(databasePath, transport);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = Identify(composition, 104);
            var operations = new SupervisionOperationalFacade(composition);
            var now = At(3);
            operations.QueueCampaignSelection(endpoint, 7, now);
            var work = composition.BusWorkScheduler.BeginNext(endpoint.Bus, now)!;

            await new PhysicalPriorityWorkRunner(composition, operations)
                .ExecuteAsync(work, now);

            AssertFailure(composition, "CampaignSelection", "Timeout", 104);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task ModbusExceptionResponseIsJournaledPropagatedAndDoesNotDisconnect()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var expected = Failure(
                ModbusTransportFailureKind.ModbusExceptionResponse,
                functionCode: 3,
                exceptionCode: 2);
            var transport = new FailingTransport(readFailure: expected);
            var composition = Compose(databasePath, transport);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = Identify(composition, 105);
            var now = At(4);
            composition.BusWorkScheduler.QueuePolling(endpoint, PollingGroup.Fast, now);
            var work = composition.BusWorkScheduler.BeginNext(endpoint.Bus, now)!;

            var actual = await Assert.ThrowsAsync<ModbusTransportFailureException>(async () =>
                await new PhysicalPollingWorkRunner(composition, supportedProtocolVersion: 7)
                    .ExecuteAsync(work, now));

            Assert.Same(expected, actual);
            Assert.True(composition.BusConnectionManager.TryGet(endpoint.Bus.Id, out _));
            Assert.Equal(TR2SessionState.Compatible, composition.FleetRegistry.GetSession(endpoint).State);
            AssertFailure(composition, "Polling", "ModbusExceptionResponse", 105);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    private static TR2Endpoint Identify(SupervisionRuntimeComposition composition, uint deviceId)
    {
        var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
        composition.FleetRegistry.SetSession(
            TR2Session.CreateCompatible(endpoint, new TR2Device(new DeviceId(deviceId))));
        return endpoint;
    }

    private static void AssertFailure(
        SupervisionRuntimeComposition composition,
        string expectedOperation,
        string expectedCategory,
        long expectedDeviceId)
    {
        using var connection = composition.Database.OpenConnection();
        using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT device_id, operation, category
            FROM communication_failure_journal
            ORDER BY failure_id DESC
            LIMIT 1;
            """;
        using var reader = command.ExecuteReader();

        Assert.True(reader.Read());
        Assert.Equal(expectedDeviceId, reader.GetInt64(0));
        Assert.Equal(expectedOperation, reader.GetString(1));
        Assert.Equal(expectedCategory, reader.GetString(2));
        Assert.False(reader.Read());
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
        FailingTransport transport)
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

        return SupervisionRuntimeCompositionRoot.Compose(
            configuration,
            new FakeFactory(transport));
    }

    private static ModbusTransportFailureException Failure(
        ModbusTransportFailureKind kind,
        byte? functionCode = null,
        byte? exceptionCode = null) =>
        new(
            kind,
            $"simulated {kind}",
            kind switch
            {
                ModbusTransportFailureKind.Timeout => new TimeoutException("timeout"),
                ModbusTransportFailureKind.Io => new IOException("io"),
                _ => new InvalidOperationException("modbus exception response")
            },
            functionCode,
            exceptionCode);

    private static DateTimeOffset At(int minute) =>
        new(2026, 9, 9, 19, minute, 0, TimeSpan.Zero);

    private static string NewDatabasePath() =>
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s4h3b-{Guid.NewGuid():N}.db");

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
        private readonly FailingTransport _transport;

        public FakeFactory(FailingTransport transport)
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
        public FakeConnection(string busId, FailingTransport transport)
        {
            BusId = busId;
            RegisterTransport = transport;
            RegisterWriteTransport = transport;
        }

        public string BusId { get; }
        public IRegisterTransport RegisterTransport { get; }
        public IRegisterWriteTransport RegisterWriteTransport { get; }
        public ValueTask DisposeAsync() => ValueTask.CompletedTask;
    }

    private sealed class FailingTransport : IRegisterTransport, IRegisterWriteTransport
    {
        private readonly Exception? _readFailure;
        private readonly Exception? _writeFailure;

        public FailingTransport(Exception? readFailure = null, Exception? writeFailure = null)
        {
            _readFailure = readFailure;
            _writeFailure = writeFailure;
        }

        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            return _readFailure is null
                ? ValueTask.FromResult(new ushort[registerCount])
                : ValueTask.FromException<ushort[]>(_readFailure);
        }

        public ValueTask WriteRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            IReadOnlyList<ushort> values,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            return _writeFailure is null
                ? ValueTask.CompletedTask
                : ValueTask.FromException(_writeFailure);
        }
    }
}
