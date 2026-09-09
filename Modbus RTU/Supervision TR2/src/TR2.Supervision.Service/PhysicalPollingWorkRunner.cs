using TR2.Application;
using TR2.Domain;
using TR2.Transport;

namespace TR2.Supervision.Service;

public sealed class PhysicalPollingWorkRunner : IPollingWorkRunner
{
    private readonly SupervisionRuntimeComposition _composition;
    private readonly SerialBusRecoveryCoordinator _recovery;
    private readonly ushort _supportedProtocolVersion;

    public PhysicalPollingWorkRunner(SupervisionRuntimeComposition composition, ushort supportedProtocolVersion)
    {
        _composition = composition ?? throw new ArgumentNullException(nameof(composition));
        _recovery = new SerialBusRecoveryCoordinator(composition);
        _supportedProtocolVersion = supportedProtocolVersion;
    }

    public async ValueTask<PollingWorkExecutionResult> ExecuteAsync(ScheduledBusWork work, DateTimeOffset observedAt, CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(work);
        cancellationToken.ThrowIfCancellationRequested();
        if (work.Kind != BusWorkKind.Polling || work.PollingGroup is null)
            throw new InvalidOperationException("The work item is not a polling work item.");

        try
        {
            if (!_composition.BusConnectionManager.TryGet(work.Endpoint.Bus.Id, out var connection) || connection is null)
                return new PollingWorkExecutionResult(false);

            var executor = new PollingExecutor(connection.RegisterTransport, _supportedProtocolVersion);
            try
            {
                var readSet = await executor.ExecuteAsync(work, cancellationToken).ConfigureAwait(false);
                ApplyReadSet(work.Endpoint, readSet, observedAt);
                var session = _composition.FleetRegistry.GetSession(work.Endpoint);
                return new PollingWorkExecutionResult(session.State == TR2SessionState.Compatible);
            }
            catch (ModbusTransportFailureException exception)
            {
                await RecordFailureAsync(work.Endpoint, CommunicationOperation.Polling, observedAt, exception, cancellationToken).ConfigureAwait(false);
                switch (exception.Kind)
                {
                    case ModbusTransportFailureKind.Timeout:
                        MarkTelemetryUnavailable(work.Endpoint);
                        return new PollingWorkExecutionResult(false);
                    case ModbusTransportFailureKind.Io:
                        MarkTelemetryUnavailable(work.Endpoint);
                        await _recovery.MarkDisconnectedAsync(work.Endpoint.Bus, cancellationToken).ConfigureAwait(false);
                        return new PollingWorkExecutionResult(false);
                    default:
                        throw;
                }
            }
        }
        finally
        {
            _composition.BusWorkScheduler.Complete(work.Endpoint.Bus, work.WorkId);
        }
    }

    private ValueTask RecordFailureAsync(TR2Endpoint endpoint, CommunicationOperation operation, DateTimeOffset observedAt, ModbusTransportFailureException exception, CancellationToken cancellationToken) =>
        _composition.CommunicationJournal.RecordFailureAsync(endpoint, operation, CommunicationFailureClassifier.Classify(exception), observedAt, exception, cancellationToken);

    private void ApplyReadSet(TR2Endpoint endpoint, PollingReadSet readSet, DateTimeOffset observedAt)
    {
        if (readSet.B0Session is not null)
            _composition.FleetRegistry.SetSession(readSet.B0Session);

        var session = _composition.FleetRegistry.GetSession(endpoint);
        if (session.State != TR2SessionState.Compatible || session.Device is null)
            return;

        var deviceId = session.Device.DeviceId;
        if (readSet.B1 is not null) _composition.TelemetrySnapshotRegistry.ReceiveSystemState(deviceId, readSet.B1, observedAt);
        if (readSet.B2 is not null) _composition.TelemetrySnapshotRegistry.ReceiveTimeState(deviceId, readSet.B2, observedAt);
        if (readSet.B3 is not null) _composition.TelemetrySnapshotRegistry.ReceiveVibrationState(deviceId, readSet.B3, observedAt);
        if (readSet.B4 is not null) _composition.TelemetrySnapshotRegistry.ReceiveConfigurationState(deviceId, readSet.B4, observedAt);
        if (readSet.B7 is not null) _composition.TelemetrySnapshotRegistry.ReceiveDiagnosticState(deviceId, readSet.B7, observedAt);
    }

    private void MarkTelemetryUnavailable(TR2Endpoint endpoint)
    {
        var session = _composition.FleetRegistry.GetSession(endpoint);
        if (session.State == TR2SessionState.Compatible && session.Device is not null)
            _composition.TelemetrySnapshotRegistry.MarkUnavailable(session.Device.DeviceId);
    }
}
