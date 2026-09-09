using TR2.Application;
using TR2.Domain;
using TR2.Protocol;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class PhysicalB5CommandWorkRunnerTests
{
    [Fact]
    public async Task SuccessfulPhysicalCommandWritesPrepareThenSubmitExactlyOnce()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var transport = new ScriptedTransport();
            var composition = Compose(databasePath, transport);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var (operations, endpoint, deviceId, work, now) = await QueueCommandAsync(composition);
            var runner = new PhysicalPriorityWorkRunner(composition, operations);

            await runner.ExecuteAsync(work, now);

            Assert.Equal(2, transport.Writes.Count);
            Assert.Equal(B5CommandRequest.StartAddress, transport.Writes[0].StartAddress);
            Assert.Equal(B5CommandRequest.ControlAddress, transport.Writes[1].StartAddress);
            Assert.Equal(new ushort[] { B5CommandRequest.SubmitControlValue }, transport.Writes[1].Values);
            Assert.Equal(CommandTransactionState.Submitted,
                composition.CommandCoordinatorRegistry.Get(deviceId).ActiveTransaction!.State);

            composition.BusWorkScheduler.QueuePriority(endpoint, BusWorkKind.ExplicitRefresh, now);
            Assert.NotNull(composition.BusWorkScheduler.BeginNext(endpoint.Bus, now));
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task PrepareTimeoutDoesNotSubmitAndLeavesTransactionPrepared()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var transport = new ScriptedTransport
            {
                FailureOnWriteCall = 1,
                Failure = Failure(ModbusTransportFailureKind.Timeout)
            };
            var composition = Compose(databasePath, transport);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var (operations, endpoint, deviceId, work, now) = await QueueCommandAsync(composition);
            var runner = new PhysicalPriorityWorkRunner(composition, operations);

            await runner.ExecuteAsync(work, now);

            Assert.Single(transport.Writes);
            Assert.Equal(B5CommandRequest.StartAddress, transport.Writes[0].StartAddress);
            Assert.Equal(CommandTransactionState.Prepared,
                composition.CommandCoordinatorRegistry.Get(deviceId).ActiveTransaction!.State);
            Assert.True(composition.BusConnectionManager.TryGet(endpoint.Bus.Id, out _));
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task SubmitTimeoutBecomesAmbiguousWithoutReplayAndKeepsConnection()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var transport = new ScriptedTransport
            {
                FailureOnWriteCall = 2,
                Failure = Failure(ModbusTransportFailureKind.Timeout)
            };
            var composition = Compose(databasePath, transport);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var (operations, endpoint, deviceId, work, now) = await QueueCommandAsync(composition);
            var runner = new PhysicalPriorityWorkRunner(composition, operations);

            await runner.ExecuteAsync(work, now);

            Assert.Equal(2, transport.Writes.Count);
            Assert.Equal(B5CommandRequest.StartAddress, transport.Writes[0].StartAddress);
            Assert.Equal(B5CommandRequest.ControlAddress, transport.Writes[1].StartAddress);
            Assert.Equal(CommandTransactionState.Ambiguous,
                composition.CommandCoordinatorRegistry.Get(deviceId).ActiveTransaction!.State);
            Assert.True(composition.BusConnectionManager.TryGet(endpoint.Bus.Id, out _));
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task SubmitIoFailureBecomesAmbiguousDisconnectsBusAndNeverReplays()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var transport = new ScriptedTransport
            {
                FailureOnWriteCall = 2,
                Failure = Failure(ModbusTransportFailureKind.Io)
            };
            var composition = Compose(databasePath, transport);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var (operations, endpoint, deviceId, work, now) = await QueueCommandAsync(composition);
            var runner = new PhysicalPriorityWorkRunner(composition, operations);

            await runner.ExecuteAsync(work, now);

            Assert.Equal(2, transport.Writes.Count);
            Assert.Equal(CommandTransactionState.Ambiguous,
                composition.CommandCoordinatorRegistry.Get(deviceId).ActiveTransaction!.State);
            Assert.False(composition.BusConnectionManager.TryGet(endpoint.Bus.Id, out _));
            Assert.Equal(TR2SessionState.Unidentified, composition.FleetRegistry.GetSession(endpoint).State);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    private static async Task<(
        SupervisionOperationalFacade Operations,
        TR2Endpoint Endpoint,
        DeviceId DeviceId,
        ScheduledBusWork Work,
        DateTimeOffset Now)> QueueCommandAsync(SupervisionRuntimeComposition composition)
    {
        var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
        var deviceId = new DeviceId(42);
        var now = new DateTimeOffset(2026, 9, 9, 18, 0, 0, TimeSpan.Zero);

        composition.FleetRegistry.SetSession(
            TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));

        var operations = new SupervisionOperationalFacade(composition);
        var queued = await operations.QueueCommandAsync(
            endpoint,
            "request-1",
            new B5CommandIntent(
                Code: 5,
                Param1: 11,
                Param2: 22,
                Param3: 0x12345678,
                ConfirmKey: 0x55AA),
            now);

        var work = composition.BusWorkScheduler.BeginNext(endpoint.Bus, now)
            ?? throw new InvalidOperationException("Expected queued B5 command work.");

        Assert.Equal(queued.Work.WorkId, work.WorkId);
        return (operations, endpoint, deviceId, work, now);
    }

    private static SupervisionRuntimeComposition Compose(
        string databasePath,
        ScriptedTransport transport)
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

    private static ModbusTransportFailureException Failure(ModbusTransportFailureKind kind) =>
        new(
            kind,
            $"simulated {kind}",
            kind == ModbusTransportFailureKind.Timeout
                ? new TimeoutException("simulated timeout")
                : new IOException("simulated I/O failure"));

    private static string NewDatabasePath() =>
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s4g2-{Guid.NewGuid():N}.db");

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
        private readonly ScriptedTransport _transport;

        public FakeFactory(ScriptedTransport transport)
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
        public FakeConnection(string busId, ScriptedTransport transport)
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

    private sealed class ScriptedTransport : IRegisterTransport, IRegisterWriteTransport
    {
        public List<(ushort StartAddress, ushort[] Values)> Writes { get; } = [];
        public int? FailureOnWriteCall { get; init; }
        public Exception? Failure { get; init; }

        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            return ValueTask.FromResult(new ushort[registerCount]);
        }

        public ValueTask WriteRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            IReadOnlyList<ushort> values,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            Writes.Add((startAddress, values.ToArray()));

            if (FailureOnWriteCall == Writes.Count && Failure is not null)
            {
                return ValueTask.FromException(Failure);
            }

            return ValueTask.CompletedTask;
        }
    }
}
