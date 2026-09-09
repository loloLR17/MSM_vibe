using TR2.Domain;
using TR2.Protocol;
using Xunit;

namespace TR2.Application.Tests;

public sealed class B5ReconciliationServiceTests
{
    private static readonly DateTimeOffset ObservedAt =
        new(2026, 9, 9, 10, 30, 0, TimeSpan.Zero);

    [Fact]
    public async Task Terminal_evidence_is_journaled_before_active_transaction_is_released()
    {
        var journal = new RecordingJournal();
        var coordinator = Coordinator(journal);
        RestoreAmbiguous(coordinator);
        var service = new B5ReconciliationService(
            new StubReader(State(lastTransactionId: 42, lastStatusFinal: 4)));

        var result = await service.ReconcileAsync(Endpoint(), coordinator, ObservedAt);

        Assert.Equal(B5ReconciliationOutcome.TerminalEvidence, result.Decision.Outcome);
        Assert.Null(coordinator.ActiveTransaction);
        var entry = Assert.Single(journal.Entries);
        Assert.Equal(CommandTransactionJournalEventKind.TerminalEvidenceObserved, entry.Kind);
        Assert.Equal(new TransactionId(42), entry.TransactionId);
        Assert.Equal(ObservedAt, entry.ObservedAt);
    }

    [Theory]
    [InlineData(2, B5ReconciliationOutcome.StillNonTerminal)]
    [InlineData(3, B5ReconciliationOutcome.StillNonTerminal)]
    public async Task Nonterminal_matching_evidence_keeps_transaction_blocked(
        ushort status,
        B5ReconciliationOutcome expected)
    {
        var journal = new RecordingJournal();
        var coordinator = Coordinator(journal);
        RestoreAmbiguous(coordinator);
        var service = new B5ReconciliationService(
            new StubReader(State(activeTransactionId: 42, status: status)));

        var result = await service.ReconcileAsync(Endpoint(), coordinator, ObservedAt);

        Assert.Equal(expected, result.Decision.Outcome);
        Assert.NotNull(coordinator.ActiveTransaction);
        Assert.Equal(CommandTransactionState.Ambiguous, coordinator.ActiveTransaction!.State);
        Assert.Empty(journal.Entries);
    }

    [Fact]
    public async Task Insufficient_evidence_keeps_transaction_blocked()
    {
        var journal = new RecordingJournal();
        var coordinator = Coordinator(journal);
        RestoreAmbiguous(coordinator);
        var service = new B5ReconciliationService(
            new StubReader(State(activeTransactionId: 41, status: 4)));

        var result = await service.ReconcileAsync(Endpoint(), coordinator, ObservedAt);

        Assert.Equal(B5ReconciliationOutcome.InsufficientEvidence, result.Decision.Outcome);
        Assert.NotNull(coordinator.ActiveTransaction);
        Assert.Empty(journal.Entries);
    }

    [Fact]
    public async Task Journal_failure_does_not_release_terminal_transaction()
    {
        var journal = new RecordingJournal { FailAppend = true };
        var coordinator = Coordinator(journal);
        RestoreAmbiguous(coordinator);
        var service = new B5ReconciliationService(
            new StubReader(State(lastTransactionId: 42, lastStatusFinal: 4)));

        await Assert.ThrowsAsync<IOException>(async () =>
            await service.ReconcileAsync(Endpoint(), coordinator, ObservedAt));

        Assert.NotNull(coordinator.ActiveTransaction);
        Assert.Equal(CommandTransactionState.Ambiguous, coordinator.ActiveTransaction!.State);
    }

    [Fact]
    public async Task Read_failure_does_not_mutate_active_transaction()
    {
        var journal = new RecordingJournal();
        var coordinator = Coordinator(journal);
        RestoreAmbiguous(coordinator);
        var service = new B5ReconciliationService(new StubReader(failRead: true));

        await Assert.ThrowsAsync<IOException>(async () =>
            await service.ReconcileAsync(Endpoint(), coordinator, ObservedAt));

        Assert.NotNull(coordinator.ActiveTransaction);
        Assert.Empty(journal.Entries);
    }

    private static CommandCoordinator Coordinator(ICommandTransactionJournal journal) =>
        new(new DeviceId(1001), new ReservationStore(), journal);

    private static void RestoreAmbiguous(CommandCoordinator coordinator) =>
        coordinator.RestoreRecoveredAmbiguous(
            new CommandTransaction(
                coordinator.DeviceId,
                new TransactionId(42),
                "START_ACQUISITION",
                CommandTransactionState.Ambiguous));

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
        private readonly B5CommandState? _state;
        private readonly bool _failRead;

        public StubReader(B5CommandState state)
        {
            _state = state;
        }

        public StubReader(bool failRead)
        {
            _failRead = failRead;
        }

        public ValueTask<B5CommandState> ReadAsync(
            TR2Endpoint endpoint,
            CancellationToken cancellationToken = default)
        {
            if (_failRead)
            {
                throw new IOException("Injected B5 read failure.");
            }

            return ValueTask.FromResult(_state!);
        }
    }

    private sealed class RecordingJournal : ICommandTransactionJournal
    {
        public List<CommandTransactionJournalEvent> Entries { get; } = [];
        public bool FailAppend { get; init; }

        public ValueTask AppendAsync(
            CommandTransactionJournalEvent entry,
            CancellationToken cancellationToken = default)
        {
            if (FailAppend)
            {
                throw new IOException("Injected journal failure.");
            }

            Entries.Add(entry);
            return ValueTask.CompletedTask;
        }

        public ValueTask<IReadOnlyList<CommandTransactionJournalEvent>> ReadAsync(
            DeviceId deviceId,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromResult<IReadOnlyList<CommandTransactionJournalEvent>>(
                Entries.Where(entry => entry.DeviceId == deviceId).ToArray());
    }

    private sealed class ReservationStore : ICommandTransactionReservationStore
    {
        public ValueTask<TransactionId?> GetLastAllocatedAsync(
            DeviceId deviceId,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromResult<TransactionId?>(null);

        public ValueTask PersistAllocationAsync(
            DeviceId deviceId,
            TransactionId transactionId,
            CancellationToken cancellationToken = default) =>
            ValueTask.CompletedTask;
    }
}
