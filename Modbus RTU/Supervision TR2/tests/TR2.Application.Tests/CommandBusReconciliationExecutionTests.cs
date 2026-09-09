using TR2.Domain;
using TR2.Protocol;
using Xunit;

namespace TR2.Application.Tests;

public sealed class CommandBusReconciliationExecutionTests
{
    private static readonly DateTimeOffset Now =
        new(2026, 9, 9, 10, 30, 0, TimeSpan.Zero);

    [Fact]
    public async Task Reconciliation_work_executes_B5_service_and_completes_bus_work()
    {
        var scheduler = new BusWorkScheduler();
        var reader = new StubB5Reader(State(lastTransactionId: 1, lastStatusFinal: 4));
        var service = new B5ReconciliationService(reader);
        var orchestrator = new CommandBusOrchestrator(scheduler, service);
        var coordinator = await AmbiguousCoordinatorAsync();
        var endpoint = Endpoint();
        var work = orchestrator.QueueReconciliation(endpoint, coordinator, Now);

        var started = await orchestrator.BeginNextAsync(endpoint.Bus, Now);
        var result = await orchestrator.ExecuteReconciliationAsync(started!, Now);

        Assert.Equal(B5ReconciliationOutcome.TerminalEvidence, result.Decision.Outcome);
        Assert.Null(coordinator.ActiveTransaction);
        Assert.Null(scheduler.BeginNext(endpoint.Bus, Now));
    }

    [Fact]
    public async Task Insufficient_evidence_keeps_transaction_ambiguous_and_completes_work()
    {
        var scheduler = new BusWorkScheduler();
        var reader = new StubB5Reader(State(activeTransactionId: 99, status: 4));
        var service = new B5ReconciliationService(reader);
        var orchestrator = new CommandBusOrchestrator(scheduler, service);
        var coordinator = await AmbiguousCoordinatorAsync();
        var endpoint = Endpoint();
        var work = orchestrator.QueueReconciliation(endpoint, coordinator, Now);

        var started = await orchestrator.BeginNextAsync(endpoint.Bus, Now);
        var result = await orchestrator.ExecuteReconciliationAsync(started!, Now);

        Assert.Equal(B5ReconciliationOutcome.InsufficientEvidence, result.Decision.Outcome);
        Assert.Equal(CommandTransactionState.Ambiguous, coordinator.ActiveTransaction?.State);
        Assert.Null(scheduler.BeginNext(endpoint.Bus, Now));
    }

    [Fact]
    public async Task Read_failure_releases_bus_but_keeps_transaction_ambiguous()
    {
        var scheduler = new BusWorkScheduler();
        var service = new B5ReconciliationService(new ThrowingB5Reader());
        var orchestrator = new CommandBusOrchestrator(scheduler, service);
        var coordinator = await AmbiguousCoordinatorAsync();
        var endpoint = Endpoint();
        var work = orchestrator.QueueReconciliation(endpoint, coordinator, Now);

        var started = await orchestrator.BeginNextAsync(endpoint.Bus, Now);

        await Assert.ThrowsAsync<IOException>(async () =>
            await orchestrator.ExecuteReconciliationAsync(started!, Now));

        Assert.Equal(CommandTransactionState.Ambiguous, coordinator.ActiveTransaction?.State);
        Assert.Null(scheduler.BeginNext(endpoint.Bus, Now));
    }

    [Fact]
    public async Task Non_reconciliation_work_cannot_be_executed_as_reconciliation()
    {
        var scheduler = new BusWorkScheduler();
        var service = new B5ReconciliationService(new StubB5Reader(State()));
        var orchestrator = new CommandBusOrchestrator(scheduler, service);
        var endpoint = Endpoint();
        var work = scheduler.QueuePriority(endpoint, BusWorkKind.ExplicitRefresh, Now);
        var started = scheduler.BeginNext(endpoint.Bus, Now);

        await Assert.ThrowsAsync<InvalidOperationException>(async () =>
            await orchestrator.ExecuteReconciliationAsync(started!, Now));

        scheduler.Complete(endpoint.Bus, work.WorkId);
    }

    private static async Task<CommandCoordinator> AmbiguousCoordinatorAsync()
    {
        var coordinator = new CommandCoordinator(
            new DeviceId(1001),
            new InMemoryReservationStore());
        await coordinator.PrepareAsync("START");
        coordinator.MarkSubmitted();
        coordinator.MarkAmbiguous();
        return coordinator;
    }

    private static TR2Endpoint Endpoint() =>
        new(new SerialBus("RS485-A"), new ModbusAddress(10));

    private static B5CommandState State(
        ushort activeTransactionId = 0,
        ushort status = 0,
        ushort lastTransactionId = 0,
        ushort lastStatusFinal = 0) =>
        new(
            0, 0, 0, 0, 0, 0, 0,
            0, activeTransactionId, status, 0, 0, 0,
            0, lastTransactionId, lastStatusFinal, 0, 0);

    private sealed class StubB5Reader : IB5CommandStateReader
    {
        private readonly B5CommandState _state;

        public StubB5Reader(B5CommandState state) => _state = state;

        public ValueTask<B5CommandState> ReadAsync(
            TR2Endpoint endpoint,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromResult(_state);
    }

    private sealed class ThrowingB5Reader : IB5CommandStateReader
    {
        public ValueTask<B5CommandState> ReadAsync(
            TR2Endpoint endpoint,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromException<B5CommandState>(new IOException("simulated read failure"));
    }

    private sealed class InMemoryReservationStore : ICommandTransactionReservationStore
    {
        private TransactionId? _last;

        public ValueTask<TransactionId?> GetLastAllocatedAsync(
            DeviceId deviceId,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromResult(_last);

        public ValueTask PersistAllocationAsync(
            DeviceId deviceId,
            TransactionId transactionId,
            CancellationToken cancellationToken = default)
        {
            _last = transactionId;
            return ValueTask.CompletedTask;
        }
    }
}
