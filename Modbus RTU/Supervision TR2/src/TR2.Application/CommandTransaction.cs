using TR2.Domain;

namespace TR2.Application;

public enum CommandTransactionState
{
    Prepared,
    Submitted,
    Ambiguous
}

public sealed record CommandTransaction(
    DeviceId DeviceId,
    TransactionId TransactionId,
    string RequestIdentity,
    CommandTransactionState State)
{
    public CommandTransaction MarkSubmitted() =>
        State == CommandTransactionState.Prepared
            ? this with { State = CommandTransactionState.Submitted }
            : throw new InvalidOperationException("Only a prepared transaction can be submitted.");

    public CommandTransaction MarkAmbiguous() =>
        State == CommandTransactionState.Submitted
            ? this with { State = CommandTransactionState.Ambiguous }
            : throw new InvalidOperationException("Only a submitted transaction can become ambiguous.");

    public CommandTransaction MarkAmbiguousAfterSubmitAttempt() =>
        State is CommandTransactionState.Prepared or CommandTransactionState.Submitted
            ? this with { State = CommandTransactionState.Ambiguous }
            : throw new InvalidOperationException("Only a prepared or submitted transaction can become ambiguous after a submit attempt.");
}
