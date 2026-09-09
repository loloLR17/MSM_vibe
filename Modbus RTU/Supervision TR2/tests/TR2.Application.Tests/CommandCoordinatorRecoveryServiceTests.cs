using TR2.Domain;
using Xunit;

namespace TR2.Application.Tests;

public sealed class CommandCoordinatorRecoveryServiceTests
{
    private static readonly DateTimeOffset T0 =
        new(2026, 9, 9, 10, 0, 0, TimeSpan.Zero);

    [Theory]
    [InlineData(CommandTransactionJournalEventKind.Prepared)]
    [InlineData(CommandTransactionJournalEventKind.Submitted)]
    [InlineData(CommandTransactionJournalEventKind.Ambiguous)]
    public async Task Nonterminal_journal_state_is_restored_as_ambiguous(
        CommandTransactionJournalEventKind finalKind)
    {
        var deviceId = new DeviceId(1001);
        var journal = new RecordingJournal();
        AppendLifecycle(journal, deviceId, finalKind);
        var coordinator = new CommandCoordinator(
            deviceId,
            new InMemoryReservationStore(new TransactionId(42)),
            journal);
        var recovery = new CommandCoordinatorRecoveryService(journal);

        var recovered = await recovery.RestoreAsync(coordinator);

        Assert.NotNull(recovered);
        Assert.Equal(new TransactionId(42), recovered!.TransactionId);
        Assert.Equal("START_ACQUISITION", recovered.RequestIdentity);
        Assert.Equal(CommandTransactionState.Ambiguous, recovered.State);
        Assert.Same(recovered, coordinator.ActiveTransaction);
        Assert.Equal(0, journal.AppendCountDuringRecovery);
    }

    [Fact]
    public async Task Terminal_evidence_leaves_no_active_transaction()
    {
        var deviceId = new DeviceId(1001);
        var journal = new RecordingJournal();
        AppendLifecycle(journal, deviceId, CommandTransactionJournalEventKind.Submitted);
        journal.Seed(new CommandTransactionJournalEvent(
            deviceId,
            new TransactionId(42),
            "START_ACQUISITION",
            CommandTransactionJournalEventKind.TerminalEvidenceObserved,
            T0.AddSeconds(2)));
        var coordinator = new CommandCoordinator(
            deviceId,
            new InMemoryReservationStore(new TransactionId(42)),
            journal);

        var recovered = await new CommandCoordinatorRecoveryService(journal)
            .RestoreAsync(coordinator);

        Assert.Null(recovered);
        Assert.Null(coordinator.ActiveTransaction);
    }

    [Fact]
    public async Task Recovered_ambiguous_transaction_blocks_new_commands()
    {
        var deviceId = new DeviceId(1001);
        var journal = new RecordingJournal();
        AppendLifecycle(journal, deviceId, CommandTransactionJournalEventKind.Prepared);
        var coordinator = new CommandCoordinator(
            deviceId,
            new InMemoryReservationStore(new TransactionId(42)),
            journal);
        await new CommandCoordinatorRecoveryService(journal).RestoreAsync(coordinator);

        await Assert.ThrowsAsync<InvalidOperationException>(async () =>
            await coordinator.PrepareAsync("STOP_ACQUISITION", T0.AddMinutes(1)));
    }

    [Fact]
    public async Task Malformed_journal_identity_is_rejected()
    {
        var deviceId = new DeviceId(1001);
        var journal = new RecordingJournal();
        journal.Seed(new CommandTransactionJournalEvent(
            deviceId,
            new TransactionId(42),
            "START_ACQUISITION",
            CommandTransactionJournalEventKind.Prepared,
            T0));
        journal.Seed(new CommandTransactionJournalEvent(
            deviceId,
            new TransactionId(42),
            "STOP_ACQUISITION",
            CommandTransactionJournalEventKind.Submitted,
            T0.AddSeconds(1)));
        var coordinator = new CommandCoordinator(
            deviceId,
            new InMemoryReservationStore(new TransactionId(42)),
            journal);

        await Assert.ThrowsAsync<InvalidDataException>(async () =>
            await new CommandCoordinatorRecoveryService(journal).RestoreAsync(coordinator));
    }

    [Fact]
    public async Task New_prepared_before_previous_terminal_is_rejected()
    {
        var deviceId = new DeviceId(1001);
        var journal = new RecordingJournal();
        journal.Seed(new CommandTransactionJournalEvent(
            deviceId,
            new TransactionId(41),
            "SYNC_TIME",
            CommandTransactionJournalEventKind.Prepared,
            T0));
        journal.Seed(new CommandTransactionJournalEvent(
            deviceId,
            new TransactionId(42),
            "START_ACQUISITION",
            CommandTransactionJournalEventKind.Prepared,
            T0.AddSeconds(1)));
        var coordinator = new CommandCoordinator(
            deviceId,
            new InMemoryReservationStore(new TransactionId(42)),
            journal);

        await Assert.ThrowsAsync<InvalidDataException>(async () =>
            await new CommandCoordinatorRecoveryService(journal).RestoreAsync(coordinator));
    }

    private static void AppendLifecycle(
        RecordingJournal journal,
        DeviceId deviceId,
        CommandTransactionJournalEventKind finalKind)
    {
        journal.Seed(new CommandTransactionJournalEvent(
            deviceId,
            new TransactionId(42),
            "START_ACQUISITION",
            CommandTransactionJournalEventKind.Prepared,
            T0));

        if (finalKind is CommandTransactionJournalEventKind.Prepared)
        {
            return;
        }

        journal.Seed(new CommandTransactionJournalEvent(
            deviceId,
            new TransactionId(42),
            "START_ACQUISITION",
            CommandTransactionJournalEventKind.Submitted,
            T0.AddSeconds(1)));

        if (finalKind is CommandTransactionJournalEventKind.Ambiguous)
        {
            journal.Seed(new CommandTransactionJournalEvent(
                deviceId,
                new TransactionId(42),
                "START_ACQUISITION",
                CommandTransactionJournalEventKind.Ambiguous,
                T0.AddSeconds(2)));
        }
    }

    private sealed class RecordingJournal : ICommandTransactionJournal
    {
        private readonly List<CommandTransactionJournalEvent> _entries = [];
        private bool _reading;

        public int AppendCountDuringRecovery { get; private set; }

        public void Seed(CommandTransactionJournalEvent entry) => _entries.Add(entry);

        public ValueTask AppendAsync(
            CommandTransactionJournalEvent entry,
            CancellationToken cancellationToken = default)
        {
            if (_reading)
            {
                AppendCountDuringRecovery++;
            }

            _entries.Add(entry);
            return ValueTask.CompletedTask;
        }

        public ValueTask<IReadOnlyList<CommandTransactionJournalEvent>> ReadAsync(
            DeviceId deviceId,
            CancellationToken cancellationToken = default)
        {
            _reading = true;
            IReadOnlyList<CommandTransactionJournalEvent> result = _entries
                .Where(entry => entry.DeviceId == deviceId)
                .ToArray();
            _reading = false;
            return ValueTask.FromResult(result);
        }
    }

    private sealed class InMemoryReservationStore : ICommandTransactionReservationStore
    {
        private TransactionId? _lastAllocated;

        public InMemoryReservationStore(TransactionId? lastAllocated = null)
        {
            _lastAllocated = lastAllocated;
        }

        public ValueTask<TransactionId?> GetLastAllocatedAsync(
            DeviceId deviceId,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromResult(_lastAllocated);

        public ValueTask PersistAllocationAsync(
            DeviceId deviceId,
            TransactionId transactionId,
            CancellationToken cancellationToken = default)
        {
            _lastAllocated = transactionId;
            return ValueTask.CompletedTask;
        }
    }
}
