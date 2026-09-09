namespace TR2.Application;

public sealed record ReconciliationCycleResult(
    B5ReconciliationResult Reconciliation,
    ScheduledBusWork? NextWork);
