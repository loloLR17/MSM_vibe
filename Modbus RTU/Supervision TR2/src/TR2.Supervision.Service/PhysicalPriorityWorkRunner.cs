using TR2.Application;
using TR2.Domain;
using TR2.Protocol;
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

        switch (work.Kind)
        {
            case BusWorkKind.ExplicitRefresh:
                await ExecuteRefreshAsync(work, observedAt, cancellationToken).ConfigureAwait(false);
                return;

            case BusWorkKind.CommandTransaction:
                await ExecuteCommandAsync(work, observedAt, cancellationToken).ConfigureAwait(false);
                return;

            default:
                throw new InvalidOperationException(
                    "Physical priority runner supports explicit refresh and B5 command transaction work only.");
        }
    }

    private async ValueTask ExecuteRefreshAsync(
        ScheduledBusWork work,
        DateTimeOffset observedAt,
        CancellationToken cancellationToken)
    {
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

    private async ValueTask ExecuteCommandAsync(
        ScheduledBusWork work,
        DateTimeOffset observedAt,
        CancellationToken cancellationToken)
    {
        try
        {
            if (!_composition.BusConnectionManager.TryGet(work.Endpoint.Bus.Id, out var connection)
                || connection is null)
            {
                return;
            }

            var session = _composition.FleetRegistry.GetSession(work.Endpoint);
            if (session.State != TR2SessionState.Compatible || session.Device is null)
            {
                throw new InvalidOperationException(
                    "Physical B5 execution requires a compatible identified TR2 session.");
            }

            var request = _operations.GetCommandRequest(work);
            var coordinator = _composition.CommandCoordinatorRegistry.Get(session.Device.DeviceId);
            var writer = new B5CommandWriter(connection.RegisterWriteTransport);
            var service = new B5CommandExecutionService(writer);

            try
            {
                await service
                    .ExecuteAsync(work.Endpoint, coordinator, request, observedAt, cancellationToken)
                    .ConfigureAwait(false);
            }
            catch (ModbusTransportFailureException exception)
                when (exception.Kind == ModbusTransportFailureKind.Timeout)
            {
                // If the timeout occurred during submit, B5CommandExecutionService has already
                // transitioned the durable supervision transaction to Ambiguous. Never replay here.
            }
            catch (ModbusTransportFailureException exception)
                when (exception.Kind == ModbusTransportFailureKind.Io)
            {
                // If the I/O failure occurred during submit, the transaction is already Ambiguous.
                // Closing the physical bus forces reconnect + fresh B0 before further operations.
                await _recovery
                    .MarkDisconnectedAsync(work.Endpoint.Bus, cancellationToken)
                    .ConfigureAwait(false);
            }
        }
        finally
        {
            _composition.BusWorkScheduler.Complete(work.Endpoint.Bus, work.WorkId);
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
