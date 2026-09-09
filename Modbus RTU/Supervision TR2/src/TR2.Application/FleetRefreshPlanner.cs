using TR2.Domain;

namespace TR2.Application;

public sealed record ScheduledBlockRefresh(
    ScheduledBusWork Work,
    TR2RegisterBlock Block);

public sealed class FleetRefreshPlanner
{
    private readonly BusWorkScheduler _scheduler;

    public FleetRefreshPlanner(BusWorkScheduler scheduler)
    {
        ArgumentNullException.ThrowIfNull(scheduler);
        _scheduler = scheduler;
    }

    public IReadOnlyList<ScheduledBlockRefresh> QueuePostReconnectRefresh(
        TR2Session session,
        DateTimeOffset dueAt)
    {
        ArgumentNullException.ThrowIfNull(session);

        if (session.State != TR2SessionState.Compatible || session.Device is null)
        {
            throw new InvalidOperationException(
                "Post-reconnect state refresh requires a compatible identified session.");
        }

        return TR2PollingPlan.PostReconnectRefreshBlocks
            .Select(block => new ScheduledBlockRefresh(
                _scheduler.QueuePriority(
                    session.Endpoint,
                    BusWorkKind.ExplicitRefresh,
                    dueAt),
                block))
            .ToArray();
    }
}
