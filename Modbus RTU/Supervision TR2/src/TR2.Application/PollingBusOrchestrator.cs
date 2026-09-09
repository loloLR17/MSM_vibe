using TR2.Domain;

namespace TR2.Application;

public sealed class PollingBusOrchestrator
{
    private readonly BusWorkScheduler _scheduler;
    private readonly PollingExecutor _executor;

    public PollingBusOrchestrator(BusWorkScheduler scheduler, PollingExecutor executor)
    {
        ArgumentNullException.ThrowIfNull(scheduler);
        ArgumentNullException.ThrowIfNull(executor);
        _scheduler = scheduler;
        _executor = executor;
    }

    public ScheduledBusWork Queue(
        TR2Endpoint endpoint,
        PollingGroup group,
        DateTimeOffset dueAt)
    {
        ArgumentNullException.ThrowIfNull(endpoint);
        return _scheduler.QueuePolling(endpoint, group, dueAt);
    }

    public ScheduledBusWork? BeginNext(SerialBus bus, DateTimeOffset observedAt)
    {
        ArgumentNullException.ThrowIfNull(bus);
        return _scheduler.BeginNext(bus, observedAt);
    }

    public async ValueTask<PollingReadSet> ExecuteAsync(
        ScheduledBusWork work,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(work);

        try
        {
            return await _executor.ExecuteAsync(work, cancellationToken);
        }
        finally
        {
            _scheduler.Complete(work.Endpoint.Bus, work.WorkId);
        }
    }
}
