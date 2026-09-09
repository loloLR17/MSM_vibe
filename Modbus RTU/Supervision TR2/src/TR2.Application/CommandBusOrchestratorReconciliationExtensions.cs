namespace TR2.Application;

public static class CommandBusOrchestratorReconciliationExtensions
{
    public static async ValueTask<ReconciliationCycleResult> ExecuteReconciliationCycleAsync(
        this CommandBusOrchestrator orchestrator,
        ScheduledBusWork work,
        DateTimeOffset observedAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(orchestrator);
        ArgumentNullException.ThrowIfNull(work);

        var reconciliation = await orchestrator.ExecuteReconciliationAsync(
            work,
            observedAt,
            cancellationToken);

        return new ReconciliationCycleResult(
            reconciliation,
            NextWork: null);
    }
}
