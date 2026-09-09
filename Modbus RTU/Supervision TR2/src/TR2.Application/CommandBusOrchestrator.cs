using TR2.Domain;

namespace TR2.Application;

public sealed class CommandBusOrchestrator
{
    private readonly BusWorkScheduler _scheduler;
    private readonly B5ReconciliationService? _reconciliationService;
    private readonly Dictionary<long, CommandCoordinator> _coordinatorsByWorkId = [];

    public CommandBusOrchestrator(BusWorkScheduler scheduler)
        : this(scheduler, null)
    {
    }

    public CommandBusOrchestrator(
        BusWorkScheduler scheduler,
        B5ReconciliationService? reconciliationService)
    {
        ArgumentNullException.ThrowIfNull(scheduler);
        _scheduler = scheduler;
        _reconciliationService = reconciliationService;
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

    public async ValueTask<B5ReconciliationResult> ExecuteReconciliationAsync(
        ScheduledBusWork work,
        DateTimeOffset observedAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(work);

        if (work.Kind != BusWorkKind.TransactionReconciliation)
        {
            throw new InvalidOperationException("The active work item is not a transaction reconciliation.");
        }

        if (_reconciliationService is null)
        {
            throw new InvalidOperationException("A B5 reconciliation service is required to execute reconciliation work.");
        }

        if (!_coordinatorsByWorkId.TryGetValue(work.WorkId, out var coordinator))
        {
            throw new InvalidOperationException("No command coordinator is associated with this reconciliation work item.");
        }

        try
        {
            return await _reconciliationService.ReconcileAsync(
                work.Endpoint,
                coordinator,
                observedAt,
                cancellationToken);
        }
        finally
        {
            Complete(work.Endpoint.Bus, work.WorkId);
        }
    }

    public void Complete(SerialBus bus, long workId)
    {
        _scheduler.Complete(bus, workId);
        _coordinatorsByWorkId.Remove(workId);
    }
}
