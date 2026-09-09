using TR2.Domain;

namespace TR2.Application;

public sealed class CommandCoordinator
{
    private readonly ICommandTransactionReservationStore _store;
    private readonly ICommandTransactionJournal? _journal;

    public CommandCoordinator(DeviceId deviceId, ICommandTransactionReservationStore store)
        : this(deviceId, store, null)
    {
    }

    public CommandCoordinator(
        DeviceId deviceId,
        ICommandTransactionReservationStore store,
        ICommandTransactionJournal? journal)
    {
        ArgumentNullException.ThrowIfNull(store);
        DeviceId = deviceId;
        _store = store;
        _journal = journal;
    }

    public DeviceId DeviceId { get; }

    public CommandTransaction? ActiveTransaction { get; private set; }

    public ValueTask<CommandTransaction> PrepareAsync(
        string requestIdentity,
        CancellationToken cancellationToken = default)
    {
        EnsureJournalTimestampNotRequired();
        return PrepareCoreAsync(requestIdentity, null, cancellationToken);
    }

    public ValueTask<CommandTransaction> PrepareAsync(
        string requestIdentity,
        DateTimeOffset observedAt,
        CancellationToken cancellationToken = default) =>
        PrepareCoreAsync(requestIdentity, observedAt, cancellationToken);

    public CommandTransaction MarkSubmitted()
    {
        EnsureJournalTimestampNotRequired();
        var active = RequireActive();
        ActiveTransaction = active.MarkSubmitted();
        return ActiveTransaction;
    }

    public async ValueTask<CommandTransaction> MarkSubmittedAsync(
        DateTimeOffset observedAt,
        CancellationToken cancellationToken = default)
    {
        var active = RequireActive();
        var submitted = active.MarkSubmitted();
        await AppendJournalAsync(submitted, CommandTransactionJournalEventKind.Submitted, observedAt, cancellationToken);
        ActiveTransaction = submitted;
        return submitted;
    }

    public CommandTransaction MarkAmbiguous()
    {
        EnsureJournalTimestampNotRequired();
        var active = RequireActive();
        ActiveTransaction = active.MarkAmbiguous();
        return ActiveTransaction;
    }

    public async ValueTask<CommandTransaction> MarkAmbiguousAsync(
        DateTimeOffset observedAt,
        CancellationToken cancellationToken = default)
    {
        var active = RequireActive();
        var ambiguous = active.MarkAmbiguous();
        await AppendJournalAsync(ambiguous, CommandTransactionJournalEventKind.Ambiguous, observedAt, cancellationToken);
        ActiveTransaction = ambiguous;
        return ambiguous;
    }

    public CommandTransaction MarkAmbiguousAfterSubmitAttempt()
    {
        EnsureJournalTimestampNotRequired();
        var active = RequireActive();
        ActiveTransaction = active.MarkAmbiguousAfterSubmitAttempt();
        return ActiveTransaction;
    }

    public async ValueTask<CommandTransaction> MarkAmbiguousAfterSubmitAttemptAsync(
        DateTimeOffset observedAt,
        CancellationToken cancellationToken = default)
    {
        var active = RequireActive();
        var ambiguous = active.MarkAmbiguousAfterSubmitAttempt();
        await AppendJournalAsync(ambiguous, CommandTransactionJournalEventKind.Ambiguous, observedAt, cancellationToken);
        ActiveTransaction = ambiguous;
        return ambiguous;
    }

    public void ResolveTerminal(TransactionId transactionId)
    {
        EnsureJournalTimestampNotRequired();
        RequireMatchingActive(transactionId);
        ActiveTransaction = null;
    }

    public async ValueTask ResolveTerminalAsync(
        TransactionId transactionId,
        DateTimeOffset observedAt,
        CancellationToken cancellationToken = default)
    {
        var active = RequireMatchingActive(transactionId);
        await AppendJournalAsync(active, CommandTransactionJournalEventKind.TerminalEvidenceObserved, observedAt, cancellationToken);
        ActiveTransaction = null;
    }

    public void RestoreRecoveredAmbiguous(CommandTransaction transaction)
    {
        ArgumentNullException.ThrowIfNull(transaction);

        if (transaction.DeviceId != DeviceId)
        {
            throw new InvalidOperationException("Recovered transaction belongs to another device.");
        }

        if (transaction.State != CommandTransactionState.Ambiguous)
        {
            throw new InvalidOperationException("Only an ambiguous transaction may be restored after supervision recovery.");
        }

        if (ActiveTransaction is not null)
        {
            throw new InvalidOperationException("A nonterminal transaction is already active for this device.");
        }

        ActiveTransaction = transaction;
    }

    private async ValueTask<CommandTransaction> PrepareCoreAsync(
        string requestIdentity,
        DateTimeOffset? observedAt,
        CancellationToken cancellationToken)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(requestIdentity);

        if (_journal is not null && observedAt is null)
        {
            throw new InvalidOperationException("A supervision timestamp is required when command journaling is enabled.");
        }

        if (ActiveTransaction is not null)
        {
            throw new InvalidOperationException("A nonterminal transaction is already active for this device.");
        }

        var lastAllocated = await _store.GetLastAllocatedAsync(DeviceId, cancellationToken);
        if (lastAllocated is { Value: ushort.MaxValue })
        {
            throw new InvalidOperationException("The lifetime-strict transaction_id space is exhausted for this device.");
        }

        var nextValue = lastAllocated is null
            ? (ushort)1
            : checked((ushort)(lastAllocated.Value.Value + 1));
        var transactionId = new TransactionId(nextValue);

        await _store.PersistAllocationAsync(DeviceId, transactionId, cancellationToken);

        var prepared = new CommandTransaction(
            DeviceId,
            transactionId,
            requestIdentity,
            CommandTransactionState.Prepared);

        if (observedAt is not null)
        {
            await AppendJournalAsync(prepared, CommandTransactionJournalEventKind.Prepared, observedAt.Value, cancellationToken);
        }

        ActiveTransaction = prepared;
        return prepared;
    }

    private async ValueTask AppendJournalAsync(
        CommandTransaction transaction,
        CommandTransactionJournalEventKind kind,
        DateTimeOffset observedAt,
        CancellationToken cancellationToken)
    {
        if (_journal is null)
        {
            return;
        }

        await _journal.AppendAsync(
            new CommandTransactionJournalEvent(
                transaction.DeviceId,
                transaction.TransactionId,
                transaction.RequestIdentity,
                kind,
                observedAt),
            cancellationToken);
    }

    private void EnsureJournalTimestampNotRequired()
    {
        if (_journal is not null)
        {
            throw new InvalidOperationException("Use the timestamped asynchronous transition when command journaling is enabled.");
        }
    }

    private CommandTransaction RequireMatchingActive(TransactionId transactionId)
    {
        var active = RequireActive();
        if (active.TransactionId != transactionId)
        {
            throw new InvalidOperationException("Terminal evidence does not match the active transaction.");
        }

        return active;
    }

    private CommandTransaction RequireActive() =>
        ActiveTransaction
        ?? throw new InvalidOperationException("No active command transaction exists for this device.");
}
