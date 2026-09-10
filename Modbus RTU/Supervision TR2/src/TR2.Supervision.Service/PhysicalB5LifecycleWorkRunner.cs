using System.Collections.Concurrent;
using TR2.Application;
using TR2.Domain;
using TR2.Protocol;
using TR2.Transport;

namespace TR2.Supervision.Service;

public sealed class PhysicalB5LifecycleWorkRunner : IPriorityWorkRunner
{
    private sealed record MonitoringContext(DeviceId DeviceId, CommandCoordinator Coordinator, DateTimeOffset TimeoutAt);
    private sealed record ReconciliationContext(DeviceId DeviceId, TransactionId TransactionId, CommandCoordinator Coordinator);
    private readonly record struct ReconciliationKey(DeviceId DeviceId, TransactionId TransactionId);

    private readonly SupervisionRuntimeComposition _composition;
    private readonly PhysicalPriorityWorkRunner _inner;
    private readonly RuntimeB5LifecyclePolicy? _policy;
    private readonly SerialBusRecoveryCoordinator _recovery;
    private readonly ConcurrentDictionary<long, MonitoringContext> _monitoringByWorkId = new();
    private readonly ConcurrentDictionary<long, ReconciliationContext> _reconciliationByWorkId = new();
    private readonly ConcurrentDictionary<ReconciliationKey, byte> _activeReconciliations = new();

    public PhysicalB5LifecycleWorkRunner(SupervisionRuntimeComposition composition, SupervisionOperationalFacade operations)
    {
        _composition = composition ?? throw new ArgumentNullException(nameof(composition));
        ArgumentNullException.ThrowIfNull(operations);
        _inner = new PhysicalPriorityWorkRunner(composition, operations);
        _policy = composition.Configuration.B5;
        _recovery = new SerialBusRecoveryCoordinator(composition);
    }

    public void ObserveCompatibleSession(TR2Endpoint endpoint, DateTimeOffset observedAt)
    {
        ArgumentNullException.ThrowIfNull(endpoint);
        if (_policy is null) return;
        var session = _composition.FleetRegistry.GetSession(endpoint);
        if (session.State != TR2SessionState.Compatible || session.Device is null) return;
        if (!_composition.CommandCoordinatorRegistry.TryGet(session.Device.DeviceId, out var coordinator)
            || coordinator?.ActiveTransaction is not { State: CommandTransactionState.Ambiguous } transaction) return;
        QueueReconciliationIfNeeded(endpoint, session.Device.DeviceId, coordinator, transaction.TransactionId, observedAt + _policy.ReconciliationInterval);
    }

    public async ValueTask ExecuteAsync(ScheduledBusWork work, DateTimeOffset observedAt, CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(work);
        cancellationToken.ThrowIfCancellationRequested();
        switch (work.Kind)
        {
            case BusWorkKind.CommandPostSubmitMonitoring:
                await ExecuteMonitoringAsync(work, observedAt, cancellationToken).ConfigureAwait(false);
                return;
            case BusWorkKind.TransactionReconciliation:
                await ExecuteReconciliationAsync(work, observedAt, cancellationToken).ConfigureAwait(false);
                return;
            default:
                await _inner.ExecuteAsync(work, observedAt, cancellationToken).ConfigureAwait(false);
                if (work.Kind == BusWorkKind.CommandTransaction && _policy is not null) QueueInitialMonitoringIfSubmitted(work.Endpoint, observedAt);
                return;
        }
    }

    private void QueueInitialMonitoringIfSubmitted(TR2Endpoint endpoint, DateTimeOffset observedAt)
    {
        var session = _composition.FleetRegistry.GetSession(endpoint);
        if (session.State != TR2SessionState.Compatible || session.Device is null) return;
        if (!_composition.CommandCoordinatorRegistry.TryGet(session.Device.DeviceId, out var coordinator)
            || coordinator?.ActiveTransaction?.State != CommandTransactionState.Submitted) return;
        var timeoutAt = observedAt + _policy!.PostSubmitTimeout;
        QueueMonitoring(endpoint, session.Device.DeviceId, coordinator, observedAt + _policy.PostSubmitPollInterval, timeoutAt);
    }

    private async ValueTask ExecuteMonitoringAsync(ScheduledBusWork work, DateTimeOffset observedAt, CancellationToken cancellationToken)
    {
        if (_policy is null)
        {
            _composition.BusWorkScheduler.Complete(work.Endpoint.Bus, work.WorkId);
            throw new InvalidOperationException("B5 post-submit monitoring work cannot execute when runtime B5 lifecycle policy is disabled.");
        }
        if (!_monitoringByWorkId.TryRemove(work.WorkId, out var context))
        {
            _composition.BusWorkScheduler.Complete(work.Endpoint.Bus, work.WorkId);
            throw new InvalidOperationException("No B5 post-submit monitoring context is registered for this work item.");
        }
        var session = _composition.FleetRegistry.GetSession(work.Endpoint);
        if (session.State != TR2SessionState.Compatible || session.Device is null || session.Device.DeviceId != context.DeviceId
            || !_composition.BusConnectionManager.TryGet(work.Endpoint.Bus.Id, out var connection) || connection is null)
        {
            _composition.BusWorkScheduler.Complete(work.Endpoint.Bus, work.WorkId);
            await RequeueOrMarkAmbiguousAsync(work.Endpoint, context, observedAt, cancellationToken).ConfigureAwait(false);
            return;
        }
        var monitor = new B5PostSubmitMonitor(new B5Reader(connection.RegisterTransport));
        B5PostSubmitResult result;
        try
        {
            result = await monitor.ObserveAsync(work.Endpoint, context.Coordinator, context.TimeoutAt, observedAt, cancellationToken).ConfigureAwait(false);
        }
        catch (ModbusTransportFailureException exception)
        {
            _composition.BusWorkScheduler.Complete(work.Endpoint.Bus, work.WorkId);
            if (exception.Kind == ModbusTransportFailureKind.Io)
                await _recovery.MarkDisconnectedAsync(work.Endpoint.Bus, cancellationToken).ConfigureAwait(false);
            if (exception.Kind is ModbusTransportFailureKind.Timeout or ModbusTransportFailureKind.Io)
            {
                if (context.Coordinator.ActiveTransaction?.State == CommandTransactionState.Submitted)
                    QueueMonitoring(work.Endpoint, context.DeviceId, context.Coordinator, observedAt + _policy.PostSubmitPollInterval, context.TimeoutAt);
                else if (context.Coordinator.ActiveTransaction is { State: CommandTransactionState.Ambiguous } ambiguous)
                    QueueReconciliationIfNeeded(work.Endpoint, context.DeviceId, context.Coordinator, ambiguous.TransactionId, observedAt + _policy.ReconciliationInterval);
                return;
            }
            throw;
        }
        _composition.BusWorkScheduler.Complete(work.Endpoint.Bus, work.WorkId);
        if (result.Outcome == B5PostSubmitOutcome.Pending)
            QueueMonitoring(work.Endpoint, context.DeviceId, context.Coordinator, observedAt + _policy.PostSubmitPollInterval, context.TimeoutAt);
        else if (result.Outcome == B5PostSubmitOutcome.TimedOutAmbiguous
            && context.Coordinator.ActiveTransaction is { State: CommandTransactionState.Ambiguous } ambiguous)
            QueueReconciliationIfNeeded(work.Endpoint, context.DeviceId, context.Coordinator, ambiguous.TransactionId, observedAt + _policy.ReconciliationInterval);
    }

    private async ValueTask ExecuteReconciliationAsync(ScheduledBusWork work, DateTimeOffset observedAt, CancellationToken cancellationToken)
    {
        if (_policy is null)
        {
            _composition.BusWorkScheduler.Complete(work.Endpoint.Bus, work.WorkId);
            throw new InvalidOperationException("B5 reconciliation work cannot execute when runtime B5 lifecycle policy is disabled.");
        }
        if (!_reconciliationByWorkId.TryRemove(work.WorkId, out var context))
        {
            _composition.BusWorkScheduler.Complete(work.Endpoint.Bus, work.WorkId);
            throw new InvalidOperationException("No B5 reconciliation context is registered for this work item.");
        }
        var key = new ReconciliationKey(context.DeviceId, context.TransactionId);
        if (context.Coordinator.ActiveTransaction is not { State: CommandTransactionState.Ambiguous } active || active.TransactionId != context.TransactionId)
        {
            _composition.BusWorkScheduler.Complete(work.Endpoint.Bus, work.WorkId);
            _activeReconciliations.TryRemove(key, out _);
            return;
        }
        var session = _composition.FleetRegistry.GetSession(work.Endpoint);
        if (session.State != TR2SessionState.Compatible || session.Device is null || session.Device.DeviceId != context.DeviceId
            || !_composition.BusConnectionManager.TryGet(work.Endpoint.Bus.Id, out var connection) || connection is null)
        {
            _composition.BusWorkScheduler.Complete(work.Endpoint.Bus, work.WorkId);
            _activeReconciliations.TryRemove(key, out _);
            return;
        }
        var service = new B5ReconciliationService(new B5Reader(connection.RegisterTransport));
        B5ReconciliationResult result;
        try
        {
            result = await service.ReconcileAsync(work.Endpoint, context.Coordinator, observedAt, cancellationToken).ConfigureAwait(false);
        }
        catch (ModbusTransportFailureException exception)
        {
            _composition.BusWorkScheduler.Complete(work.Endpoint.Bus, work.WorkId);
            if (exception.Kind == ModbusTransportFailureKind.Io)
            {
                await _recovery.MarkDisconnectedAsync(work.Endpoint.Bus, cancellationToken).ConfigureAwait(false);
                _activeReconciliations.TryRemove(key, out _);
                return;
            }
            if (exception.Kind == ModbusTransportFailureKind.Timeout)
            {
                if (context.Coordinator.ActiveTransaction?.State == CommandTransactionState.Ambiguous)
                    QueueReconciliationWork(work.Endpoint, context.DeviceId, context.Coordinator, context.TransactionId, observedAt + _policy.ReconciliationInterval);
                else _activeReconciliations.TryRemove(key, out _);
                return;
            }
            _activeReconciliations.TryRemove(key, out _);
            throw;
        }
        _composition.BusWorkScheduler.Complete(work.Endpoint.Bus, work.WorkId);
        if (result.Decision.Outcome == B5ReconciliationOutcome.TerminalEvidence || context.Coordinator.ActiveTransaction?.State != CommandTransactionState.Ambiguous)
        {
            _activeReconciliations.TryRemove(key, out _);
            return;
        }
        QueueReconciliationWork(work.Endpoint, context.DeviceId, context.Coordinator, context.TransactionId, observedAt + _policy.ReconciliationInterval);
    }

    private async ValueTask RequeueOrMarkAmbiguousAsync(TR2Endpoint endpoint, MonitoringContext context, DateTimeOffset observedAt, CancellationToken cancellationToken)
    {
        if (context.Coordinator.ActiveTransaction?.State != CommandTransactionState.Submitted) return;
        if (observedAt >= context.TimeoutAt)
        {
            var ambiguous = await context.Coordinator.MarkAmbiguousAsync(observedAt, cancellationToken).ConfigureAwait(false);
            QueueReconciliationIfNeeded(endpoint, context.DeviceId, context.Coordinator, ambiguous.TransactionId, observedAt + _policy!.ReconciliationInterval);
            return;
        }
        QueueMonitoring(endpoint, context.DeviceId, context.Coordinator, observedAt + _policy!.PostSubmitPollInterval, context.TimeoutAt);
    }

    private ScheduledBusWork QueueMonitoring(TR2Endpoint endpoint, DeviceId deviceId, CommandCoordinator coordinator, DateTimeOffset dueAt, DateTimeOffset timeoutAt)
    {
        return _composition.BusWorkScheduler.QueuePriority(
            endpoint,
            BusWorkKind.CommandPostSubmitMonitoring,
            dueAt,
            work =>
            {
                if (!_monitoringByWorkId.TryAdd(work.WorkId, new MonitoringContext(deviceId, coordinator, timeoutAt)))
                    throw new InvalidOperationException("A B5 post-submit monitoring context is already registered for this work item.");
            });
    }

    private void QueueReconciliationIfNeeded(TR2Endpoint endpoint, DeviceId deviceId, CommandCoordinator coordinator, TransactionId transactionId, DateTimeOffset dueAt)
    {
        if (coordinator.ActiveTransaction is not { State: CommandTransactionState.Ambiguous } active || active.TransactionId != transactionId) return;
        var key = new ReconciliationKey(deviceId, transactionId);
        if (!_activeReconciliations.TryAdd(key, 0)) return;

        try
        {
            QueueReconciliationWork(endpoint, deviceId, coordinator, transactionId, dueAt);
        }
        catch
        {
            _activeReconciliations.TryRemove(key, out _);
            throw;
        }
    }

    private void QueueReconciliationWork(TR2Endpoint endpoint, DeviceId deviceId, CommandCoordinator coordinator, TransactionId transactionId, DateTimeOffset dueAt)
    {
        _composition.BusWorkScheduler.QueuePriority(
            endpoint,
            BusWorkKind.TransactionReconciliation,
            dueAt,
            work =>
            {
                if (!_reconciliationByWorkId.TryAdd(work.WorkId, new ReconciliationContext(deviceId, transactionId, coordinator)))
                    throw new InvalidOperationException("A B5 reconciliation context is already registered for this work item.");
            });
    }
}
