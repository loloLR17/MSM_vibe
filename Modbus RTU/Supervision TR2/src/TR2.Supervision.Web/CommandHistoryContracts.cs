namespace TR2.Supervision.Web;

public enum IhmB5TransactionState
{
    Prepared,
    Submitted,
    Ambiguous,
    TerminalEvidenceObserved
}

public sealed record IhmB5TransactionReadModel(
    uint DeviceId,
    ushort TransactionId,
    string RequestIdentity,
    IhmB5TransactionState State,
    DateTimeOffset ObservedAt);

public sealed record CommandHistoryReadResponse(
    DateTimeOffset ObservedAt,
    uint DeviceId,
    IReadOnlyList<IhmB5TransactionReadModel> Transactions);

public interface ISupervisionCommandHistoryReadSource
{
    ValueTask<IReadOnlyList<IhmB5TransactionReadModel>> ReadAsync(
        uint deviceId,
        CancellationToken cancellationToken = default);
}
