using TR2.Domain;
using TR2.Protocol;

namespace TR2.Application;

public sealed class DeviceTelemetrySnapshotRegistry
{
    private readonly Dictionary<DeviceId, DeviceTelemetrySnapshots> _snapshots = [];

    public DeviceTelemetrySnapshots Get(DeviceId deviceId) =>
        _snapshots.TryGetValue(deviceId, out var snapshots)
            ? snapshots
            : DeviceTelemetrySnapshots.Empty(deviceId);

    public DeviceTelemetrySnapshots ReceiveSystemState(
        DeviceId deviceId,
        B1SystemState value,
        DateTimeOffset receivedAt)
    {
        var updated = Get(deviceId).ReceiveSystemState(value, receivedAt);
        _snapshots[deviceId] = updated;
        return updated;
    }

    public DeviceTelemetrySnapshots ReceiveTimeState(
        DeviceId deviceId,
        B2TimeState value,
        DateTimeOffset receivedAt)
    {
        var updated = Get(deviceId).ReceiveTimeState(value, receivedAt);
        _snapshots[deviceId] = updated;
        return updated;
    }

    public DeviceTelemetrySnapshots ReceiveVibrationState(
        DeviceId deviceId,
        B3VibrationSupervision value,
        DateTimeOffset receivedAt)
    {
        var updated = Get(deviceId).ReceiveVibrationState(value, receivedAt);
        _snapshots[deviceId] = updated;
        return updated;
    }

    public DeviceTelemetrySnapshots MarkUnavailable(DeviceId deviceId)
    {
        var updated = Get(deviceId).MarkUnavailable();
        _snapshots[deviceId] = updated;
        return updated;
    }
}
