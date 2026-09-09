using TR2.Domain;

namespace TR2.Application;

public sealed class CommandCoordinatorRecoveryService
{
    private readonly ICommandTransactionJournal _journal;

    public CommandCoordinatorRecoveryService(ICommandTransactionJournal journal)
    {
        ArgumentNullException.ThrowIfNull(journal);
        _journal = journal;
    }

    public async ValueTask<CommandTransaction?> RestoreAsync(
        CommandCoordinator coordinator,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(coordinator);

        var entries = await _journal.ReadAsync(coordinator.DeviceId, cancellationToken);
        CommandTransaction? active = null;

        foreach (var entry in entries)
        {
            if (entry.DeviceId != coordinator.DeviceId)
            {
                throw new InvalidDataException("The command journal returned an event for another device.");
            }

            switch (entry.Kind)
            {
                case CommandTransactionJournalEventKind.Prepared:
                    if (active is not null)
                    {
                        throw new InvalidDataException("A new prepared command appears before the previous command became terminal.");
                    }

                    active = new CommandTransaction(
                        entry.DeviceId,
                        entry.TransactionId,
                        entry.RequestIdentity,
                        CommandTransactionState.Prepared);
                    break;

                case CommandTransactionJournalEventKind.Submitted:
                    active = RequireMatching(active, entry, CommandTransactionState.Prepared)
                        .MarkSubmitted();
                    break;

                case CommandTransactionJournalEventKind.Ambiguous:
                    active = RequireMatching(active, entry, CommandTransactionState.Submitted)
                        .MarkAmbiguous();
                    break;

                case CommandTransactionJournalEventKind.TerminalEvidenceObserved:
                    RequireMatching(active, entry);
                    active = null;
                    break;

                default:
                    throw new InvalidDataException($"Unsupported command journal event kind: {entry.Kind}.");
            }
        }

        if (active is null)
        {
            return null;
        }

        var recovered = active with { State = CommandTransactionState.Ambiguous };
        coordinator.RestoreRecoveredAmbiguous(recovered);
        return recovered;
    }

    private static CommandTransaction RequireMatching(
        CommandTransaction? active,
        CommandTransactionJournalEvent entry,
        CommandTransactionState? requiredState = null)
    {
        if (active is null)
        {
            throw new InvalidDataException("A command journal transition has no preceding prepared transaction.");
        }

        if (active.TransactionId != entry.TransactionId
            || !string.Equals(active.RequestIdentity, entry.RequestIdentity, StringComparison.Ordinal))
        {
            throw new InvalidDataException("A command journal transition does not match the active transaction identity.");
        }

        if (requiredState is not null && active.State != requiredState.Value)
        {
            throw new InvalidDataException(
                $"Command journal transition {entry.Kind} is invalid from state {active.State}.");
        }

        return active;
    }
}
