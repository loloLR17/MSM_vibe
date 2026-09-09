using TR2.Domain;

namespace TR2.Application;

public interface ICommandTransactionJournal
{
    ValueTask AppendAsync(
        CommandTransactionJournalEvent entry,
        CancellationToken cancellationToken = default);

    ValueTask<IReadOnlyList<CommandTransactionJournalEvent>> ReadAsync(
        DeviceId deviceId,
        CancellationToken cancellationToken = default);
}
