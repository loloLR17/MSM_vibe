using TR2.Domain;
using TR2.Protocol;
using Xunit;

namespace TR2.Application.Tests;

public sealed class CommandPostSubmitSchedulingTests
{
    private static readonly DateTimeOffset T0 =
        new(2026, 9, 9, 12, 0, 0, TimeSpan.Zero);

    [Fact]
    public async Task Queued_monitoring_executes_through_scheduler_and_releases_bus()
    {
        var bus = new SerialBus("RS485-A");
        var endpoint = new TR2Endpoint(bus, new ModbusAddress(10));
        var coordinator = await CreateSubmittedCoordinatorAsync();
        var reader = new StubB5Reader(CreateState(activeTransactionId: 1, status: 3));
        var scheduler = new BusWorkScheduler();
        var orchestrator = new CommandBusOrchestrator(
            scheduler,
            null,
            null,
            new B5PostSubmitMonitor(reader));

        var queued = orchestrator.QueuePostSubmitMonitoring(
            endpoint,
            coordinator,
            T0,
            T0.AddSeconds(10));
        var active = orchestrator.BeginNext(bus, T0);

        Assert.Equal(queued, active);
        Assert.Equal(BusWorkKind.CommandPostSubmitMonitoring, active!.Kind);

        var result = await orchestrator.ExecutePostSubmitMonitoringAsync(
            active,
            T0.AddSeconds(1));

        Assert.Equal(B5PostSubmitOutcome.Pending, result.Outcome);
        Assert.Equal(CommandTransactionState.Submitted, coordinator.ActiveTransaction?.State);

        var polling = scheduler.QueuePolling(endpoint, PollingGroup.Fast, T0);
        Assert.Equal(polling, scheduler.BeginNext(bus, T0.AddSeconds(2)));
    }

    [Fact]
    public async Task Terminal_monitoring_releases_transaction_and_bus()
    {
        var bus = new SerialBus("RS485-A");
        var endpoint = new TR2Endpoint(bus, new ModbusAddress(10));
        var coordinator = await CreateSubmittedCoordinatorAsync();
        var scheduler = new BusWorkScheduler();
        var orchestrator = new CommandBusOrchestrator(
            scheduler,
            null,
            null,
            new B5PostSubmitMonitor(
                new StubB5Reader(CreateState(lastTransactionId: 1, lastStatusFinal: 4))));

        var queued = orchestrator.QueuePostSubmitMonitoring(
            endpoint,
            coordinator,
            T0,
            T0.AddSeconds(10));
        var active = orchestrator.BeginNext(bus, T0)!;

        var result = await orchestrator.ExecutePostSubmitMonitoringAsync(
            active,
            T0.AddSeconds(1));

        Assert.Equal(B5PostSubmitOutcome.TerminalEvidence, result.Outcome);
        Assert.Null(coordinator.ActiveTransaction);
        Assert.Null(scheduler.BeginNext(bus, T0.AddSeconds(2)));
    }

    [Fact]
    public async Task Timeout_monitoring_marks_transaction_ambiguous()
    {
        var bus = new SerialBus("RS485-A");
        var endpoint = new TR2Endpoint(bus, new ModbusAddress(10));
        var coordinator = await CreateSubmittedCoordinatorAsync();
        var scheduler = new BusWorkScheduler();
        var orchestrator = new CommandBusOrchestrator(
            scheduler,
            null,
            null,
            new B5PostSubmitMonitor(
                new StubB5Reader(CreateState(activeTransactionId: 1, status: 3))));

        var queued = orchestrator.QueuePostSubmitMonitoring(
            endpoint,
            coordinator,
            T0,
            T0.AddSeconds(5));
        var active = orchestrator.BeginNext(bus, T0)!;

        var result = await orchestrator.ExecutePostSubmitMonitoringAsync(
            active,
            T0.AddSeconds(5));

        Assert.Equal(B5PostSubmitOutcome.TimedOutAmbiguous, result.Outcome);
        Assert.Equal(CommandTransactionState.Ambiguous, coordinator.ActiveTransaction?.State);
    }

    [Fact]
    public async Task Monitoring_failure_releases_bus_but_preserves_submitted_before_timeout()
    {
        var bus = new SerialBus("RS485-A");
        var endpoint = new TR2Endpoint(bus, new ModbusAddress(10));
        var coordinator = await CreateSubmittedCoordinatorAsync();
        var scheduler = new BusWorkScheduler();
        var orchestrator = new CommandBusOrchestrator(
            scheduler,
            null,
            null,
            new B5PostSubmitMonitor(new StubB5Reader(new IOException("read failed"))));

        var queued = orchestrator.QueuePostSubmitMonitoring(
            endpoint,
            coordinator,
            T0,
            T0.AddSeconds(10));
        var active = orchestrator.BeginNext(bus, T0)!;

        await Assert.ThrowsAsync<IOException>(async () =>
            await orchestrator.ExecutePostSubmitMonitoringAsync(
                active,
                T0.AddSeconds(1)));

        Assert.Equal(CommandTransactionState.Submitted, coordinator.ActiveTransaction?.State);

        var polling = scheduler.QueuePolling(endpoint, PollingGroup.Fast, T0);
        Assert.Equal(polling, scheduler.BeginNext(bus, T0.AddSeconds(2)));
    }

    [Fact]
    public async Task Cannot_queue_monitoring_for_non_submitted_transaction()
    {
        var coordinator = new CommandCoordinator(
            new DeviceId(1001),
            new InMemoryReservationStore());
        await coordinator.PrepareAsync("START");
        var orchestrator = new CommandBusOrchestrator(new BusWorkScheduler());
        var endpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(10));

        Assert.Throws<InvalidOperationException>(() =>
            orchestrator.QueuePostSubmitMonitoring(
                endpoint,
                coordinator,
                T0,
                T0.AddSeconds(10)));
    }

    private static async Task<CommandCoordinator> CreateSubmittedCoordinatorAsync()
    {
        var coordinator = new CommandCoordinator(
            new DeviceId(1001),
            new InMemoryReservationStore());
        await coordinator.PrepareAsync("START");
        coordinator.MarkSubmitted();
        return coordinator;
    }

    private static B5CommandState CreateState(
        ushort activeTransactionId = 0,
        ushort status = 0,
        ushort lastTransactionId = 0,
        ushort lastStatusFinal = 0) =>
        new(
            RequestCode: 0,
            RequestTransactionId: 0,
            RequestParam1: 0,
            RequestParam2: 0,
            RequestParam3: 0,
            RequestConfirmKey: 0,
            RequestControl: 0,
            ActiveCode: 0,
            ActiveTransactionId: activeTransactionId,
            Status: status,
            ResultCode: 0,
            ResultDetail: 0,
            EngineFlags: 0,
            LastCode: 0,
            LastTransactionId: lastTransactionId,
            LastStatusFinal: lastStatusFinal,
            LastResultCode: 0,
            LastTimestampSeconds: 0);

    private sealed class StubB5Reader : IB5CommandStateReader
    {
        private readonly B5CommandState? _state;
        private readonly Exception? _failure;

        public StubB5Reader(B5CommandState state) => _state = state;

        public StubB5Reader(Exception failure) => _failure = failure;

        public ValueTask<B5CommandState> ReadAsync(
            TR2Endpoint endpoint,
            CancellationToken cancellationToken = default)
        {
            if (_failure is not null)
            {
                throw _failure;
            }

            return ValueTask.FromResult(_state!);
        }
    }

    private sealed class InMemoryReservationStore : ICommandTransactionReservationStore
    {
        private readonly Dictionary<DeviceId, TransactionId> _last = [];

        public ValueTask<TransactionId?> GetLastAllocatedAsync(
            DeviceId deviceId,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromResult(
                _last.TryGetValue(deviceId, out var value)
                    ? (TransactionId?)value
                    : null);

        public ValueTask PersistAllocationAsync(
            DeviceId deviceId,
            TransactionId transactionId,
            CancellationToken cancellationToken = default)
        {
            _last[deviceId] = transactionId;
            return ValueTask.CompletedTask;
        }
    }
}
