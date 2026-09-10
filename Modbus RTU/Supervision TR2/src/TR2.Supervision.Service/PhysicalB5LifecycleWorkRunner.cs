using TR2.Application;
using TR2.Domain;
using TR2.Protocol;
using TR2.Transport;

namespace TR2.Supervision.Service;

public sealed class PhysicalB5LifecycleWorkRunner : IPriorityWorkRunner
{
    private sealed record MonitoringContext(
        DeviceId DeviceId,
        CommandCoordinator Coordinator,
        DateTimeOffset TimeoutAt);

    private readonly SupervisionRuntimeComposition _composition;
    private readonly PhysicalPriorityWorkRunner _inner;
    private readonly RuntimeB5LifecyclePolicy? _policy;
    private readonly SerialBusRecoveryCoordinator _recovery;
    private readonly Dictionary<long, MonitoringContext> _monitoringByWorkId = [];

    public PhysicalB5LifecycleWorkRunner(
        SupervisionRuntimeComposition composition,
        SupervisionOperationalFacade operations)
    {
        _composition = composition ?? throw new ArgumentNullException(nameof(composition));
        ArgumentNullException.ThrowIfNull(operations);
        _inner = new PhysicalPriorityWorkRunner(composition, operations);
        _policy = composition.Configuration.B5;
        _recovery = new SerialBusRecoveryCoordinator(composition);
    }

    public async ValueTask ExecuteAsync(
        ScheduledBusWork work,
        DateTimeOffset observedAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(work);
        cancellationToken.ThrowIfCancellationRequested();

        if (work.Kind != BusWorkKind.CommandPostSubmitMonitoring)
        {
            await _inner.ExecuteAsync(work, observedAt, cancellationToken).ConfigureAwait(false);

            if (work.Kind == BusWorkKind.CommandTransaction && _policy is not null)
            {
                QueueInitialMonitoringIfSubmitted(work.Endpoint, observedAt);
            }

            return;
        }

        await ExecuteMonitoringAsync(work, observedAt, cancellationToken).ConfigureAwait(false);
    }

    private void QueueInitialMonitoringIfSubmitted(TR2Endpoint endpoint, DateTimeOffset observedAt)
    {
        var session = _composition.FleetRegistry.GetSession(endpoint);
        if (session.State != TR2SessionState.Compatible || session.Device is null)
        {
            return;
        }

        if (!_composition.CommandCoordinatorRegistry.TryGet(session.Device.DeviceId, out var coordinator)
            || coordinator?.ActiveTransaction?.State != CommandTransactionState.Submitted)
        {
            return;
        }

        var timeoutAt = observedAt + _policy!.PostSubmitTimeout;
        QueueMonitoring(
            endpoint,
            session.Device.DeviceId,
            coordinator,
            observedAt + _policy.PostSubmitPollInterval,
            timeoutAt);
    }

    private async ValueTask ExecuteMonitoringAsync(
        ScheduledBusWork work,
        DateTimeOffset observedAt,
        CancellationToken cancellationToken)
    {
        if (_policy is null)
        {
            _composition.BusWorkScheduler.Complete(work.Endpoint.Bus, work.WorkId);
            throw new InvalidOperationException(
                "B5 post-submit monitoring work cannot execute when runtime B5 lifecycle policy is disabled.");
        }

        if (!_monitoringByWorkId.Remove(work.WorkId, out var context))
        {
            _composition.BusWorkScheduler.Complete(work.Endpoint.Bus, work.WorkId);
            throw new InvalidOperationException("No B5 post-submit monitoring context is registered for this work item.");
        }

        var session = _composition.FleetRegistry.GetSession(work.Endpoint);
        if (session.State != TR2SessionState.Compatible
            || session.Device is null
            || session.Device.DeviceId != context.DeviceId
            || !_composition.BusConnectionManager.TryGet(work.Endpoint.Bus.Id, out var connection)
            || connection is null)
        {
            _composition.BusWorkScheduler.Complete(work.Endpoint.Bus, work.WorkId);
            await RequeueOrMarkAmbiguousAsync(work.Endpoint, context, observedAt, cancellationToken)
                .ConfigureAwait(false);
            return;
        }

        var monitor = new B5PostSubmitMonitor(new B5Reader(connection.RegisterTransport));
        B5PostSubmitResult result;

        try
        {
            result = await monitor.ObserveAsync(
                work.Endpoint,
                context.Coordinator,
                context.TimeoutAt,
                observedAt,
                cancellationToken).ConfigureAwait(false);
        }
        catch (ModbusTransportFailureException exception)
        {
            _composition.BusWorkScheduler.Complete(work.Endpoint.Bus, work.WorkId);

            if (exception.Kind == ModbusTransportFailureKind.Io)
            {
                await _recovery.MarkDisconnectedAsync(work.Endpoint.Bus, cancellationToken)
                    .ConfigureAwait(false);
            }

            if (exception.Kind is ModbusTransportFailureKind.Timeout or ModbusTransportFailureKind.Io)
            {
                if (context.Coordinator.ActiveTransaction?.State == CommandTransactionState.Submitted)
                {
                    QueueMonitoring(
                        work.Endpoint,
                        context.DeviceId,
                        context.Coordinator,
                        observedAt + _policy.PostSubmitPollInterval,
                        context.TimeoutAt);
                }

                return;
            }

            throw;
        }

        _composition.BusWorkScheduler.Complete(work.Endpoint.Bus, work.WorkId);

        if (result.Outcome == B5PostSubmitOutcome.Pending)
        {
            QueueMonitoring(
                work.Endpoint,
                context.DeviceId,
                context.Coordinator,
                observedAt + _policy.PostSubmitPollInterval,
                context.TimeoutAt);
        }
    }

    private async ValueTask RequeueOrMarkAmbiguousAsync(
        TR2Endpoint endpoint,
        MonitoringContext context,
        DateTimeOffset observedAt,
        CancellationToken cancellationToken)
    {
        if (context.Coordinator.ActiveTransaction?.State != CommandTransactionState.Submitted)
        {
            return;
        }

        if (observedAt >= context.TimeoutAt)
        {
            await context.Coordinator.MarkAmbiguousAsync(observedAt, cancellationToken)
                .ConfigureAwait(false);
            return;
        }

        QueueMonitoring(
            endpoint,
            context.DeviceId,
            context.Coordinator,
            observedAt + _policy!.PostSubmitPollInterval,
            context.TimeoutAt);
    }

    private ScheduledBusWork QueueMonitoring(
        TR2Endpoint endpoint,
        DeviceId deviceId,
        CommandCoordinator coordinator,
        DateTimeOffset dueAt,
        DateTimeOffset timeoutAt)
    {
        var work = _composition.BusWorkScheduler.QueuePriority(
            endpoint,
            BusWorkKind.CommandPostSubmitMonitoring,
            dueAt);

        _monitoringByWorkId.Add(
            work.WorkId,
            new MonitoringContext(deviceId, coordinator, timeoutAt));
        return work;
    }
}
