using TR2.Domain;

namespace TR2.Application;

public sealed class CommandBusOrchestrator
{
    private readonly BusWorkScheduler _scheduler;
    private readonly Dictionary<long, CommandCoordinator> _coordinatorsByWorkId = [];

    public CommandBusOrchestrator(BusWorkScheduler scheduler)
    {
        ArgumentNullException.ThrowIfNull(scheduler);
        _scheduler = scheduler;
    }

    public async ValueTask<ScheduledBusWork> PrepareAndQueueAsync(
        TR2Endpoint endpoint,
        CommandCoordinator coordinator,
        string requestIdentity,
        DateTimeOffset dueAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(endpoint);
        ArgumentNullException.ThrowIfNull(coordinator);

        await coordinator.PrepareAsync(requestIdentity, dueAt, cancellationToken);

        var work = _scheduler.QueuePriority(
            endpoint,
            BusWorkKind.CommandTransaction,
            dueAt);

        _coordinatorsByWorkId.Add(work.WorkId, coordinator);
        return work;
    }

    public ScheduledBusWork QueueReconciliation(
        TR2Endpoint endpoint,
        CommandCoordinator coordinator,
        DateTimeOffset dueAt)
    {
        ArgumentNullException.ThrowIfNull(endpoint);
        ArgumentNullException.ThrowIfNull(coordinator);

        if (coordinator.ActiveTransaction?.State != CommandTransactionState.Ambiguous)
        {
            throw new InvalidOperationException(
                "Reconciliation can only be queued for an ambiguous active transaction.");
        }

        var work = _scheduler.QueuePriority(
            endpoint,
            BusWorkKind.TransactionReconciliation,
            dueAt);

        _coordinatorsByWorkId.Add(work.WorkId, coordinator);
        return work;
    }

    public ScheduledBusWork? BeginNext(SerialBus bus, DateTimeOffset observedAt)
    {
        ArgumentNullException.ThrowIfNull(bus);

        var work = _scheduler.BeginNext(bus, observedAt);
        if (work is null)
        {
            return null;
        }

        if (work.Kind == BusWorkKind.CommandTransaction
            && _coordinatorsByWorkId.TryGetValue(work.WorkId, out var coordinator))
        {
            coordinator.MarkSubmitted();
        }

        return work;
    }

    public async ValueTask<ScheduledBusWork?> BeginNextAsync(
        SerialBus bus,
        DateTimeOffset observedAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(bus);

        var work = _scheduler.BeginNext(bus, observedAt);
        if (work is null)
        {
            return null;
        }

        if (work.Kind == BusWorkKind.CommandTransaction
            && _coordinatorsByWorkId.TryGetValue(work.WorkId, out var coordinator))
        {
            await coordinator.MarkSubmittedAsync(observedAt, cancellationToken);
        }

        return work;
    }

    public void Complete(SerialBus bus, long workId)
    {
        _scheduler.Complete(bus, workId);
        _coordinatorsByWorkId.Remove(workId);
    }
}
