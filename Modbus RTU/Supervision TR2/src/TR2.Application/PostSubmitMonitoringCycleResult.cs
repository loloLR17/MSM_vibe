namespace TR2.Application;

public sealed record PostSubmitMonitoringCycleResult(
    B5PostSubmitResult Observation,
    ScheduledBusWork? NextMonitoringWork);
