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

        if (B5TransactionEvidence.HasTerminalEvidence(transaction.TransactionId, observed))
        {
            return new B5ReconciliationDecision(
                B5ReconciliationOutcome.TerminalEvidence,
                transaction.TransactionId);
        }

        if (observed.ActiveTransactionId == transaction.TransactionId.Value)
        {
            return new B5ReconciliationDecision(
                B5ReconciliationOutcome.StillNonTerminal,
                transaction.TransactionId);
        }

        return new B5ReconciliationDecision(
            B5ReconciliationOutcome.InsufficientEvidence,
            transaction.TransactionId);
    }
}
