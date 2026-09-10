using TR2.Domain;

namespace TR2.Application;

public sealed class BusWorkScheduler
{
    private readonly object _sync = new();
    private readonly List<ScheduledBusWork> _pending = [];
    private readonly Dictionary<SerialBus, ScheduledBusWork> _activeByBus = [];
    private long _nextWorkId = 1;
    private long _nextSequence;

    public ScheduledBusWork QueuePolling(
        TR2Endpoint endpoint,
        PollingGroup group,
        DateTimeOffset dueAt)
    {
        ArgumentNullException.ThrowIfNull(endpoint);
        return Queue(endpoint, BusWorkKind.Polling, group, dueAt, null);
    }

    public ScheduledBusWork QueuePriority(
        TR2Endpoint endpoint,
        BusWorkKind kind,
        DateTimeOffset dueAt)
    {
        return QueuePriority(endpoint, kind, dueAt, null);
    }

    public ScheduledBusWork QueuePriority(
        TR2Endpoint endpoint,
        BusWorkKind kind,
        DateTimeOffset dueAt,
        Action<ScheduledBusWork>? initializeBeforePublish)
    {
        ArgumentNullException.ThrowIfNull(endpoint);

        if (kind == BusWorkKind.Polling)
        {
            throw new ArgumentOutOfRangeException(nameof(kind));
        }

        return Queue(endpoint, kind, null, dueAt, initializeBeforePublish);
    }

    public ScheduledBusWork? BeginNext(SerialBus bus, DateTimeOffset observedAt)
    {
        ArgumentNullException.ThrowIfNull(bus);

        lock (_sync)
        {
            if (_activeByBus.ContainsKey(bus))
            {
                return null;
            }

            var next = _pending
                .Where(work => work.Endpoint.Bus == bus && work.DueAt <= observedAt)
                .OrderByDescending(work => work.IsPriority)
                .ThenBy(work => work.DueAt)
                .ThenBy(work => work.Sequence)
                .FirstOrDefault();

            if (next is null)
            {
                return null;
            }

            _pending.Remove(next);
            _activeByBus.Add(bus, next);
            return next;
        }
    }

    public void Complete(SerialBus bus, long workId)
    {
        ArgumentNullException.ThrowIfNull(bus);

        lock (_sync)
        {
            if (!_activeByBus.TryGetValue(bus, out var active) || active.WorkId != workId)
            {
                throw new InvalidOperationException("The work item is not active on this bus.");
            }

            _activeByBus.Remove(bus);
        }
    }

    private ScheduledBusWork Queue(
        TR2Endpoint endpoint,
        BusWorkKind kind,
        PollingGroup? group,
        DateTimeOffset dueAt,
        Action<ScheduledBusWork>? initializeBeforePublish)
    {
        lock (_sync)
        {
            var work = new ScheduledBusWork(
                _nextWorkId++,
                endpoint,
                kind,
                group,
                dueAt,
                _nextSequence++);

            initializeBeforePublish?.Invoke(work);
            _pending.Add(work);
            return work;
        }
    }
}
