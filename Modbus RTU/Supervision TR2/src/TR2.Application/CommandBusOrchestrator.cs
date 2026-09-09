using TR2.Domain;
using TR2.Protocol;

namespace TR2.Application;

public sealed class CommandBusOrchestrator
{
    private readonly BusWorkScheduler _scheduler;
    private readonly B5ReconciliationService? _reconciliationService;
    private readonly B5CommandExecutionService? _commandExecutionService;
    private readonly B5PostSubmitMonitor? _postSubmitMonitor;
    private readonly Dictionary<long, CommandCoordinator> _coordinatorsByWorkId = [];
    private readonly Dictionary<long, DateTimeOffset> _postSubmitTimeoutByWorkId = [];

    public CommandBusOrchestrator(BusWorkScheduler scheduler)
        : this(scheduler, null, null, null)
    {
    }

    public CommandBusOrchestrator(
        BusWorkScheduler scheduler,
        B5ReconciliationService? reconciliationService)
        : this(scheduler, reconciliationService, null, null)
    {
    }

    public CommandBusOrchestrator(
        BusWorkScheduler scheduler,
        B5ReconciliationService? reconciliationService,
        B5CommandExecutionService? commandExecutionService)
        : this(scheduler, reconciliationService, commandExecutionService, null)
    {
    }

    public CommandBusOrchestrator(
        BusWorkScheduler scheduler,
        B5ReconciliationService? reconciliationService,
        B5CommandExecutionService? commandExecutionService,
        B5PostSubmitMonitor? postSubmitMonitor)
    {
        ArgumentNullException.ThrowIfNull(scheduler);
        _scheduler = scheduler;
        _reconciliationService = reconciliationService;
        _commandExecutionService = commandExecutionService;
        _postSubmitMonitor = postSubmitMonitor;
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

    public ScheduledBusWork QueuePostSubmitMonitoring(
        TR2Endpoint endpoint,
        CommandCoordinator coordinator,
        DateTimeOffset dueAt,
        DateTimeOffset timeoutAt)
    {
        ArgumentNullException.ThrowIfNull(endpoint);
        ArgumentNullException.ThrowIfNull(coordinator);

        if (coordinator.ActiveTransaction?.State != CommandTransactionState.Submitted)
        {
            throw new InvalidOperationException(
                "Post-submit monitoring can only be queued for a submitted active transaction.");
        }

        var work = _scheduler.QueuePriority(
            endpoint,
            BusWorkKind.CommandPostSubmitMonitoring,
            dueAt);

        _coordinatorsByWorkId.Add(work.WorkId, coordinator);
        _postSubmitTimeoutByWorkId.Add(work.WorkId, timeoutAt);
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
        return _scheduler.BeginNext(bus, observedAt);
    }

    public ValueTask<ScheduledBusWork?> BeginNextAsync(
        SerialBus bus,
        DateTimeOffset observedAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(bus);
        return ValueTask.FromResult(_scheduler.BeginNext(bus, observedAt));
    }

    public async ValueTask ExecuteCommandAsync(
        ScheduledBusWork work,
        B5CommandRequest request,
        DateTimeOffset observedAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(work);
        ArgumentNullException.ThrowIfNull(request);

        if (work.Kind != BusWorkKind.CommandTransaction)
        {
            throw new InvalidOperationException("The active work item is not a command transaction.");
        }

        if (_commandExecutionService is null)
        {
            throw new InvalidOperationException("A B5 command execution service is required to execute command work.");
        }

        if (!_coordinatorsByWorkId.TryGetValue(work.WorkId, out var coordinator))
        {
            throw new InvalidOperationException("No command coordinator is associated with this command work item.");
        }

        try
        {
            await _commandExecutionService.ExecuteAsync(
                work.Endpoint,
                coordinator,
                request,
                observedAt,
                cancellationToken);
        }
        finally
        {
            Complete(work.Endpoint.Bus, work.WorkId);
        }
    }

    public async ValueTask<ScheduledBusWork> ExecuteCommandAndQueueMonitoringAsync(
        ScheduledBusWork work,
        B5CommandRequest request,
        DateTimeOffset observedAt,
        DateTimeOffset monitoringDueAt,
        DateTimeOffset timeoutAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(work);
        ArgumentNullException.ThrowIfNull(request);

        if (work.Kind != BusWorkKind.CommandTransaction)
        {
            throw new InvalidOperationException("The active work item is not a command transaction.");
        }

        if (_commandExecutionService is null)
        {
            throw new InvalidOperationException("A B5 command execution service is required to execute command work.");
        }

        if (!_coordinatorsByWorkId.TryGetValue(work.WorkId, out var coordinator))
        {
            throw new InvalidOperationException("No command coordinator is associated with this command work item.");
        }

        try
        {
            await _commandExecutionService.ExecuteAsync(
                work.Endpoint,
                coordinator,
                request,
                observedAt,
                cancellationToken);
        }
        finally
        {
            Complete(work.Endpoint.Bus, work.WorkId);
        }

        return QueuePostSubmitMonitoring(
            work.Endpoint,
            coordinator,
            monitoringDueAt,
            timeoutAt);
    }

    public async ValueTask<B5PostSubmitResult> ExecutePostSubmitMonitoringAsync(
        ScheduledBusWork work,
        DateTimeOffset observedAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(work);

        if (work.Kind != BusWorkKind.CommandPostSubmitMonitoring)
        {
            throw new InvalidOperationException("The active work item is not post-submit monitoring.");
        }

        if (_postSubmitMonitor is null)
        {
            throw new InvalidOperationException("A B5 post-submit monitor is required to execute monitoring work.");
        }

        if (!_coordinatorsByWorkId.TryGetValue(work.WorkId, out var coordinator)
            || !_postSubmitTimeoutByWorkId.TryGetValue(work.WorkId, out var timeoutAt))
        {
            throw new InvalidOperationException("No command monitoring context is associated with this work item.");
        }

        try
        {
            return await _postSubmitMonitor.ObserveAsync(
                work.Endpoint,
                coordinator,
                timeoutAt,
                observedAt,
                cancellationToken);
        }
        finally
        {
            Complete(work.Endpoint.Bus, work.WorkId);
        }
    }

    public async ValueTask<PostSubmitMonitoringCycleResult> ExecutePostSubmitMonitoringAndRequeueAsync(
        ScheduledBusWork work,
        DateTimeOffset observedAt,
        DateTimeOffset nextMonitoringDueAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(work);

        if (work.Kind != BusWorkKind.CommandPostSubmitMonitoring)
        {
            throw new InvalidOperationException("The active work item is not post-submit monitoring.");
        }

        if (_postSubmitMonitor is null)
        {
            throw new InvalidOperationException("A B5 post-submit monitor is required to execute monitoring work.");
        }

        if (!_coordinatorsByWorkId.TryGetValue(work.WorkId, out var coordinator)
            || !_postSubmitTimeoutByWorkId.TryGetValue(work.WorkId, out var timeoutAt))
        {
            throw new InvalidOperationException("No command monitoring context is associated with this work item.");
        }

        B5PostSubmitResult result;
        try
        {
            result = await _postSubmitMonitor.ObserveAsync(
                work.Endpoint,
                coordinator,
                timeoutAt,
                observedAt,
                cancellationToken);
        }
        finally
        {
            Complete(work.Endpoint.Bus, work.WorkId);
        }

        ScheduledBusWork? next = null;
        if (result.Outcome == B5PostSubmitOutcome.Pending)
        {
            next = QueuePostSubmitMonitoring(
                work.Endpoint,
                coordinator,
                nextMonitoringDueAt,
                timeoutAt);
        }

        return new PostSubmitMonitoringCycleResult(result, next);
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
        _postSubmitTimeoutByWorkId.Remove(workId);
    }
}
