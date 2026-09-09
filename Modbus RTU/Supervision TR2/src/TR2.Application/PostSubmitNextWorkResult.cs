namespace TR2.Application;

public sealed record PostSubmitNextWorkResult(
    B5PostSubmitResult Observation,
    ScheduledBusWork? NextWork);
