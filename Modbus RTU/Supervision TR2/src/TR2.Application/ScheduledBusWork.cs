using TR2.Domain;

namespace TR2.Application;

public enum BusWorkKind
{
    Polling,
    ExplicitRefresh,
    CommandTransaction,
    CommandPostSubmitMonitoring,
    TransactionReconciliation
}

public sealed record ScheduledBusWork(
    long WorkId,
    TR2Endpoint Endpoint,
    BusWorkKind Kind,
    PollingGroup? PollingGroup,
    DateTimeOffset DueAt,
    long Sequence)
{
    public bool IsPriority => Kind != BusWorkKind.Polling;
}
