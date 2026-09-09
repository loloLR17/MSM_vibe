using TR2.Protocol;

namespace TR2.Application;

public enum B5ReconciliationOutcome
{
    TerminalEvidence,
    StillNonTerminal,
    InsufficientEvidence
}

public sealed record B5ReconciliationDecision(
    B5ReconciliationOutcome Outcome,
    TransactionId TransactionId);

public static class B5ReconciliationDecider
{
    private const ushort StatusReceived = 1;
    private const ushort StatusAccepted = 2;
    private const ushort StatusInProgress = 3;
    private const ushort StatusSucceeded = 4;
    private const ushort StatusRefused = 5;
    private const ushort StatusFailed = 6;
    private const ushort StatusUnknownCommand = 7;
    private const ushort StatusNotAllowed = 8;

    public static B5ReconciliationDecision Decide(
        CommandTransaction transaction,
        B5CommandState observed)
    {
        ArgumentNullException.ThrowIfNull(transaction);
        ArgumentNullException.ThrowIfNull(observed);

        if (transaction.State != CommandTransactionState.Ambiguous)
        {
            throw new InvalidOperationException(
                "B5 reconciliation is only defined for an ambiguous supervision transaction.");
        }

        var transactionId = transaction.TransactionId.Value;

        if (observed.LastTransactionId == transactionId
            && IsTerminalStatus(observed.LastStatusFinal))
        {
            return new B5ReconciliationDecision(
                B5ReconciliationOutcome.TerminalEvidence,
                transaction.TransactionId);
        }

        if (observed.ActiveTransactionId == transactionId)
        {
            return IsTerminalStatus(observed.Status)
                ? new B5ReconciliationDecision(
                    B5ReconciliationOutcome.TerminalEvidence,
                    transaction.TransactionId)
                : new B5ReconciliationDecision(
                    B5ReconciliationOutcome.StillNonTerminal,
                    transaction.TransactionId);
        }

        return new B5ReconciliationDecision(
            B5ReconciliationOutcome.InsufficientEvidence,
            transaction.TransactionId);
    }

    private static bool IsTerminalStatus(ushort status) =>
        status is StatusSucceeded
            or StatusRefused
            or StatusFailed
            or StatusUnknownCommand
            or StatusNotAllowed;
}
