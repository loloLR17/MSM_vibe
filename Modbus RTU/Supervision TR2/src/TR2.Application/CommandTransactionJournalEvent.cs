using TR2.Domain;

namespace TR2.Application;

public enum CommandTransactionJournalEventKind
{
    Prepared,
    Submitted,
    Ambiguous,
    TerminalEvidenceObserved
}

public sealed record CommandTransactionJournalEvent
{
    public CommandTransactionJournalEvent(
        DeviceId deviceId,
        TransactionId transactionId,
        string requestIdentity,
        CommandTransactionJournalEventKind kind,
        DateTimeOffset observedAt)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(requestIdentity);

        DeviceId = deviceId;
        TransactionId = transactionId;
        RequestIdentity = requestIdentity;
        Kind = kind;
        ObservedAt = observedAt;
    }

    public DeviceId DeviceId { get; }

    public TransactionId TransactionId { get; }

    public string RequestIdentity { get; }

    public CommandTransactionJournalEventKind Kind { get; }

    public DateTimeOffset ObservedAt { get; }
}
