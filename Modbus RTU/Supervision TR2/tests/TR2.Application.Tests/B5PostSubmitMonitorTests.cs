using TR2.Domain;
using TR2.Protocol;
using Xunit;

namespace TR2.Application.Tests;

public sealed class B5PostSubmitMonitorTests
{
    private static readonly DateTimeOffset T0 =
        new(2026, 9, 9, 11, 0, 0, TimeSpan.Zero);

    [Fact]
    public async Task Terminal_evidence_resolves_submitted_transaction()
    {
        var coordinator = await SubmittedCoordinatorAsync();
        var monitor = new B5PostSubmitMonitor(
            new StubReader(State(lastTransactionId: 1, lastStatusFinal: 4)));

        var result = await monitor.ObserveAsync(
            Endpoint(),
            coordinator,
            T0.AddSeconds(10),
            T0.AddSeconds(2));

        Assert.Equal(B5PostSubmitOutcome.TerminalEvidence, result.Outcome);
        Assert.Null(coordinator.ActiveTransaction);
    }

    [Fact]
    public async Task Nonterminal_evidence_before_timeout_remains_pending()
    {
        var coordinator = await SubmittedCoordinatorAsync();
        var monitor = new B5PostSubmitMonitor(
            new StubReader(State(activeTransactionId: 1, status: 3)));

        var result = await monitor.ObserveAsync(
            Endpoint(),
            coordinator,
            T0.AddSeconds(10),
            T0.AddSeconds(2));

        Assert.Equal(B5PostSubmitOutcome.Pending, result.Outcome);
        Assert.Equal(CommandTransactionState.Submitted, coordinator.ActiveTransaction?.State);
    }

    [Fact]
    public async Task Missing_terminal_evidence_at_timeout_becomes_ambiguous()
    {
        var coordinator = await SubmittedCoordinatorAsync();
        var monitor = new B5PostSubmitMonitor(
            new StubReader(State(activeTransactionId: 1, status: 3)));

        var result = await monitor.ObserveAsync(
            Endpoint(),
            coordinator,
            T0.AddSeconds(10),
            T0.AddSeconds(10));

        Assert.Equal(B5PostSubmitOutcome.TimedOutAmbiguous, result.Outcome);
        Assert.Equal(CommandTransactionState.Ambiguous, coordinator.ActiveTransaction?.State);
    }

    [Fact]
    public async Task Read_failure_before_timeout_preserves_submitted_state()
    {
        var coordinator = await SubmittedCoordinatorAsync();
        var monitor = new B5PostSubmitMonitor(new ThrowingReader());

        await Assert.ThrowsAsync<IOException>(async () =>
            await monitor.ObserveAsync(
                Endpoint(),
                coordinator,
                T0.AddSeconds(10),
                T0.AddSeconds(2)));

        Assert.Equal(CommandTransactionState.Submitted, coordinator.ActiveTransaction?.State);
    }

    [Fact]
    public async Task Read_failure_at_timeout_makes_transaction_ambiguous_before_error_propagates()
    {
        var coordinator = await SubmittedCoordinatorAsync();
        var monitor = new B5PostSubmitMonitor(new ThrowingReader());

        await Assert.ThrowsAsync<IOException>(async () =>
            await monitor.ObserveAsync(
                Endpoint(),
                coordinator,
                T0.AddSeconds(10),
                T0.AddSeconds(10)));

        Assert.Equal(CommandTransactionState.Ambiguous, coordinator.ActiveTransaction?.State);
    }

    [Fact]
    public async Task Ambiguous_transaction_is_not_accepted_as_normal_post_submit_monitoring()
    {
        var coordinator = await SubmittedCoordinatorAsync();
        coordinator.MarkAmbiguous();
        var monitor = new B5PostSubmitMonitor(new StubReader(State()));

        await Assert.ThrowsAsync<InvalidOperationException>(async () =>
            await monitor.ObserveAsync(
                Endpoint(),
                coordinator,
                T0.AddSeconds(10),
                T0.AddSeconds(2)));
    }

    private static async Task<CommandCoordinator> SubmittedCoordinatorAsync()
    {
        var coordinator = new CommandCoordinator(
            new DeviceId(1001),
            new InMemoryReservationStore());
        await coordinator.PrepareAsync("START");
        coordinator.MarkSubmitted();
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

    private sealed class StubReader : IB5CommandStateReader
    {
        private readonly B5CommandState _state;

        public StubReader(B5CommandState state) => _state = state;

        public ValueTask<B5CommandState> ReadAsync(
            TR2Endpoint endpoint,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromResult(_state);
    }

    private sealed class ThrowingReader : IB5CommandStateReader
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
