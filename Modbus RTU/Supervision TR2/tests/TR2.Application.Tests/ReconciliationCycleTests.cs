using TR2.Domain;
using TR2.Protocol;
using Xunit;

namespace TR2.Application.Tests;

public sealed class ReconciliationCycleTests
{
    private static readonly DateTimeOffset T0 =
        new(2026, 9, 9, 13, 0, 0, TimeSpan.Zero);

    [Fact]
    public async Task Terminal_evidence_releases_transaction_without_scheduling_follow_up()
    {
        var (orchestrator, scheduler, endpoint, coordinator) =
            await CreateCycleAsync(CreateState(lastTransactionId: 1, lastStatusFinal: 4));

        var work = orchestrator.QueueReconciliation(endpoint, coordinator, T0);
        var active = orchestrator.BeginNext(endpoint.Bus, T0)!;
        var result = await orchestrator.ExecuteReconciliationCycleAsync(active, T0.AddSeconds(1));

        Assert.Equal(B5ReconciliationOutcome.TerminalEvidence, result.Reconciliation.Decision.Outcome);
        Assert.Null(result.NextWork);
        Assert.Null(coordinator.ActiveTransaction);
        AssertBusIsReleased(scheduler, endpoint, T0.AddSeconds(2));
    }

    [Fact]
    public async Task Still_nonterminal_keeps_ambiguous_and_does_not_requeue()
    {
        var (orchestrator, scheduler, endpoint, coordinator) =
            await CreateCycleAsync(CreateState(activeTransactionId: 1, status: 3));

        var work = orchestrator.QueueReconciliation(endpoint, coordinator, T0);
        var active = orchestrator.BeginNext(endpoint.Bus, T0)!;
        var result = await orchestrator.ExecuteReconciliationCycleAsync(active, T0.AddSeconds(1));

        Assert.Equal(B5ReconciliationOutcome.StillNonTerminal, result.Reconciliation.Decision.Outcome);
        Assert.Null(result.NextWork);
        Assert.Equal(CommandTransactionState.Ambiguous, coordinator.ActiveTransaction?.State);
        AssertBusIsReleased(scheduler, endpoint, T0.AddSeconds(2));
    }

    [Fact]
    public async Task Insufficient_evidence_keeps_ambiguous_and_does_not_requeue()
    {
        var (orchestrator, scheduler, endpoint, coordinator) =
            await CreateCycleAsync(CreateState());

        var work = orchestrator.QueueReconciliation(endpoint, coordinator, T0);
        var active = orchestrator.BeginNext(endpoint.Bus, T0)!;
        var result = await orchestrator.ExecuteReconciliationCycleAsync(active, T0.AddSeconds(1));

        Assert.Equal(B5ReconciliationOutcome.InsufficientEvidence, result.Reconciliation.Decision.Outcome);
        Assert.Null(result.NextWork);
        Assert.Equal(CommandTransactionState.Ambiguous, coordinator.ActiveTransaction?.State);
        AssertBusIsReleased(scheduler, endpoint, T0.AddSeconds(2));
    }

    [Fact]
    public async Task Read_failure_keeps_ambiguous_releases_bus_and_does_not_hide_exception()
    {
        var endpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(10));
        var coordinator = await CreateAmbiguousCoordinatorAsync();
        var scheduler = new BusWorkScheduler();
        var orchestrator = new CommandBusOrchestrator(
            scheduler,
            new B5ReconciliationService(new StubB5Reader(new IOException("read failed"))));

        var work = orchestrator.QueueReconciliation(endpoint, coordinator, T0);
        var active = orchestrator.BeginNext(endpoint.Bus, T0)!;

        await Assert.ThrowsAsync<IOException>(async () =>
            await orchestrator.ExecuteReconciliationCycleAsync(active, T0.AddSeconds(1)));

        Assert.Equal(CommandTransactionState.Ambiguous, coordinator.ActiveTransaction?.State);
        AssertBusIsReleased(scheduler, endpoint, T0.AddSeconds(2));
    }

    private static async Task<(CommandBusOrchestrator Orchestrator, BusWorkScheduler Scheduler, TR2Endpoint Endpoint, CommandCoordinator Coordinator)>
        CreateCycleAsync(B5CommandState state)
    {
        var endpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(10));
        var coordinator = await CreateAmbiguousCoordinatorAsync();
        var scheduler = new BusWorkScheduler();
        var orchestrator = new CommandBusOrchestrator(
            scheduler,
            new B5ReconciliationService(new StubB5Reader(state)));
        return (orchestrator, scheduler, endpoint, coordinator);
    }

    private static async Task<CommandCoordinator> CreateAmbiguousCoordinatorAsync()
    {
        var coordinator = new CommandCoordinator(
            new DeviceId(1001),
            new InMemoryReservationStore());
        await coordinator.PrepareAsync("START");
        coordinator.MarkSubmitted();
        coordinator.MarkAmbiguous();
        return coordinator;
    }

    private static void AssertBusIsReleased(
        BusWorkScheduler scheduler,
        TR2Endpoint endpoint,
        DateTimeOffset observedAt)
    {
        var polling = scheduler.QueuePolling(endpoint, PollingGroup.Fast, observedAt);
        Assert.Equal(polling, scheduler.BeginNext(endpoint.Bus, observedAt));
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
