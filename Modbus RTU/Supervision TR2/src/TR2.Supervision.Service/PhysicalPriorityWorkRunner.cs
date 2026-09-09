using TR2.Application;
using TR2.Domain;
using TR2.Transport;

namespace TR2.Supervision.Service;

public sealed class PhysicalPriorityWorkRunner : IPriorityWorkRunner
{
    private readonly SupervisionRuntimeComposition _composition;
    private readonly SupervisionOperationalFacade _operations;
    private readonly SerialBusRecoveryCoordinator _recovery;

    public PhysicalPriorityWorkRunner(
        SupervisionRuntimeComposition composition,
        SupervisionOperationalFacade operations)
    {
        _composition = composition ?? throw new ArgumentNullException(nameof(composition));
        _operations = operations ?? throw new ArgumentNullException(nameof(operations));
        _recovery = new SerialBusRecoveryCoordinator(composition);
    }

    public async ValueTask ExecuteAsync(
        ScheduledBusWork work,
        DateTimeOffset observedAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(work);
        cancellationToken.ThrowIfCancellationRequested();

        if (work.Kind != BusWorkKind.ExplicitRefresh)
        {
            throw new InvalidOperationException(
                "Physical priority runner currently supports explicit refresh work only.");
        }

        if (!_composition.BusConnectionManager.TryGet(work.Endpoint.Bus.Id, out var connection)
            || connection is null)
        {
            _composition.BusWorkScheduler.Complete(work.Endpoint.Bus, work.WorkId);
            return;
        }

        var refresh = _operations.GetRefresh(work);
        var executor = new ExplicitRefreshExecutor(
            _composition.BusWorkScheduler,
            connection.RegisterTransport);

        try
        {
            var readSet = await executor
                .ExecuteAsync(refresh, cancellationToken)
                .ConfigureAwait(false);

            ApplyReadSet(work.Endpoint, readSet, observedAt);
        }
        catch (ModbusTransportFailureException exception)
            when (exception.Kind == ModbusTransportFailureKind.Timeout)
        {
            MarkTelemetryUnavailable(work.Endpoint);
        }
        catch (ModbusTransportFailureException exception)
            when (exception.Kind == ModbusTransportFailureKind.Io)
        {
            MarkTelemetryUnavailable(work.Endpoint);
            await _recovery
                .MarkDisconnectedAsync(work.Endpoint.Bus, cancellationToken)
                .ConfigureAwait(false);
        }
    }

    private void ApplyReadSet(
        TR2Endpoint endpoint,
        PollingReadSet readSet,
        DateTimeOffset observedAt)
    {
        var session = _composition.FleetRegistry.GetSession(endpoint);
        if (session.State != TR2SessionState.Compatible || session.Device is null)
        {
            return;
        }

        var deviceId = session.Device.DeviceId;

        if (readSet.B1 is not null)
        {
            _composition.TelemetrySnapshotRegistry.ReceiveSystemState(
                deviceId,
                readSet.B1,
                observedAt);
        }

        if (readSet.B2 is not null)
        {
            _composition.TelemetrySnapshotRegistry.ReceiveTimeState(
                deviceId,
                readSet.B2,
                observedAt);
        }

        if (readSet.B3 is not null)
        {
            _composition.TelemetrySnapshotRegistry.ReceiveVibrationState(
                deviceId,
                readSet.B3,
                observedAt);
        }
    }

    private void MarkTelemetryUnavailable(TR2Endpoint endpoint)
    {
        var session = _composition.FleetRegistry.GetSession(endpoint);
        if (session.State == TR2SessionState.Compatible && session.Device is not null)
        {
            _composition.TelemetrySnapshotRegistry.MarkUnavailable(session.Device.DeviceId);
        }
    }
}
