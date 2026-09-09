using TR2.Domain;

namespace TR2.Application;

public sealed class PollingTelemetryPublisher
{
    private readonly FleetRegistry _fleet;
    private readonly DeviceTelemetrySnapshotRegistry _telemetry;

    public PollingTelemetryPublisher(
        FleetRegistry fleet,
        DeviceTelemetrySnapshotRegistry telemetry)
    {
        ArgumentNullException.ThrowIfNull(fleet);
        ArgumentNullException.ThrowIfNull(telemetry);
        _fleet = fleet;
        _telemetry = telemetry;
    }

    public DeviceTelemetrySnapshots? Publish(
        TR2Endpoint endpoint,
        PollingReadSet readSet,
        DateTimeOffset receivedAt)
    {
        ArgumentNullException.ThrowIfNull(endpoint);
        ArgumentNullException.ThrowIfNull(readSet);

        if (readSet.B1 is null && readSet.B2 is null && readSet.B3 is null)
        {
            return null;
        }

        var deviceId = RequireCompatibleDevice(endpoint);
        var snapshots = _telemetry.Get(deviceId);

        if (readSet.B1 is not null)
        {
            snapshots = _telemetry.ReceiveSystemState(deviceId, readSet.B1, receivedAt);
        }

        if (readSet.B2 is not null)
        {
            snapshots = _telemetry.ReceiveTimeState(deviceId, readSet.B2, receivedAt);
        }

        if (readSet.B3 is not null)
        {
            snapshots = _telemetry.ReceiveVibrationState(deviceId, readSet.B3, receivedAt);
        }

        return snapshots;
    }

    public DeviceTelemetrySnapshots? MarkUnavailable(TR2Endpoint endpoint)
    {
        ArgumentNullException.ThrowIfNull(endpoint);

        var session = _fleet.GetSession(endpoint);
        if (session.State != TR2SessionState.Compatible || session.Device is null)
        {
            return null;
        }

        return _telemetry.MarkUnavailable(session.Device.DeviceId);
    }

    private DeviceId RequireCompatibleDevice(TR2Endpoint endpoint)
    {
        var session = _fleet.GetSession(endpoint);
        if (session.State != TR2SessionState.Compatible || session.Device is null)
        {
            throw new InvalidOperationException(
                "Telemetry polling can only be published for a compatible identified TR2 session.");
        }

        return session.Device.DeviceId;
    }
}
