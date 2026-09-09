using TR2.Domain;
using TR2.Protocol;

namespace TR2.Application;

public enum B5PostSubmitOutcome
{
    Pending,
    TerminalEvidence,
    TimedOutAmbiguous
}

public sealed record B5PostSubmitResult(
    B5PostSubmitOutcome Outcome,
    B5CommandState? Observation);

public sealed class B5PostSubmitMonitor
{
    private readonly IB5CommandStateReader _reader;

    public B5PostSubmitMonitor(IB5CommandStateReader reader)
    {
        ArgumentNullException.ThrowIfNull(reader);
        _reader = reader;
    }

    public async ValueTask<B5PostSubmitResult> ObserveAsync(
        TR2Endpoint endpoint,
        CommandCoordinator coordinator,
        DateTimeOffset timeoutAt,
        DateTimeOffset observedAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(endpoint);
        ArgumentNullException.ThrowIfNull(coordinator);

        var transaction = coordinator.ActiveTransaction
            ?? throw new InvalidOperationException(
                "Post-submit monitoring requires an active supervision transaction.");

        if (transaction.State != CommandTransactionState.Submitted)
        {
            throw new InvalidOperationException(
                "Post-submit monitoring requires a submitted supervision transaction.");
        }

        B5CommandState observation;
        try
        {
            observation = await _reader.ReadAsync(endpoint, cancellationToken);
        }
        catch when (observedAt >= timeoutAt && !cancellationToken.IsCancellationRequested)
        {
            await coordinator.MarkAmbiguousAsync(observedAt, cancellationToken);
            throw;
        }

        if (B5TransactionEvidence.HasTerminalEvidence(transaction.TransactionId, observation))
        {
            await coordinator.ResolveTerminalAsync(
                transaction.TransactionId,
                observedAt,
                cancellationToken);

            return new B5PostSubmitResult(
                B5PostSubmitOutcome.TerminalEvidence,
                observation);
        }

        if (observedAt >= timeoutAt)
        {
            await coordinator.MarkAmbiguousAsync(observedAt, cancellationToken);
            return new B5PostSubmitResult(
                B5PostSubmitOutcome.TimedOutAmbiguous,
                observation);
        }

        return new B5PostSubmitResult(
            B5PostSubmitOutcome.Pending,
            observation);
    }
}
