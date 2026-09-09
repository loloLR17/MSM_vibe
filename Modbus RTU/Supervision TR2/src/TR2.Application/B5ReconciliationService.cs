using TR2.Domain;
using TR2.Protocol;

namespace TR2.Application;

public sealed record B5ReconciliationResult(
    B5CommandState Observation,
    B5ReconciliationDecision Decision);

public sealed class B5ReconciliationService
{
    private readonly IB5CommandStateReader _reader;

    public B5ReconciliationService(IB5CommandStateReader reader)
    {
        ArgumentNullException.ThrowIfNull(reader);
        _reader = reader;
    }

    public async ValueTask<B5ReconciliationResult> ReconcileAsync(
        TR2Endpoint endpoint,
        CommandCoordinator coordinator,
        DateTimeOffset observedAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(endpoint);
        ArgumentNullException.ThrowIfNull(coordinator);

        var transaction = coordinator.ActiveTransaction
            ?? throw new InvalidOperationException(
                "B5 reconciliation requires an active supervision transaction.");

        var observation = await _reader.ReadAsync(endpoint, cancellationToken);
        var decision = B5ReconciliationDecider.Decide(transaction, observation);

        if (decision.Outcome == B5ReconciliationOutcome.TerminalEvidence)
        {
            await coordinator.ResolveTerminalAsync(
                decision.TransactionId,
                observedAt,
                cancellationToken);
        }

        return new B5ReconciliationResult(observation, decision);
    }
}
