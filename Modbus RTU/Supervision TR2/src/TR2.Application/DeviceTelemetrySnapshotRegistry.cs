using TR2.Domain;
using TR2.Protocol;

namespace TR2.Application;

public sealed class DeviceTelemetrySnapshotRegistry
{
    private readonly object _sync = new();
    private readonly Dictionary<DeviceId, DeviceTelemetrySnapshots> _snapshots = [];

    public DeviceTelemetrySnapshots Get(DeviceId deviceId)
    {
        lock (_sync)
        {
            return GetUnsafe(deviceId);
        }
    }

    public DeviceTelemetrySnapshots ReceiveSystemState(DeviceId deviceId, B1SystemState value, DateTimeOffset receivedAt)
    {
        lock (_sync)
        {
            var updated = GetUnsafe(deviceId).ReceiveSystemState(value, receivedAt);
            _snapshots[deviceId] = updated;
            return updated;
        }
    }

    public DeviceTelemetrySnapshots ReceiveTimeState(DeviceId deviceId, B2TimeState value, DateTimeOffset receivedAt)
    {
        lock (_sync)
        {
            var updated = GetUnsafe(deviceId).ReceiveTimeState(value, receivedAt);
            _snapshots[deviceId] = updated;
            return updated;
        }
    }

    public DeviceTelemetrySnapshots ReceiveVibrationState(DeviceId deviceId, B3VibrationSupervision value, DateTimeOffset receivedAt)
    {
        lock (_sync)
        {
            var updated = GetUnsafe(deviceId).ReceiveVibrationState(value, receivedAt);
            _snapshots[deviceId] = updated;
            return updated;
        }
    }

    public DeviceTelemetrySnapshots ReceiveConfigurationState(DeviceId deviceId, B4ConfigurationState value, DateTimeOffset receivedAt)
    {
        lock (_sync)
        {
            var updated = GetUnsafe(deviceId).ReceiveConfigurationState(value, receivedAt);
            _snapshots[deviceId] = updated;
            return updated;
        }
    }

    public DeviceTelemetrySnapshots ReceiveDiagnosticState(DeviceId deviceId, B7DiagnosticState value, DateTimeOffset receivedAt)
    {
        lock (_sync)
        {
            var updated = GetUnsafe(deviceId).ReceiveDiagnosticState(value, receivedAt);
            _snapshots[deviceId] = updated;
            return updated;
        }
    }

    public DeviceTelemetrySnapshots MarkUnavailable(DeviceId deviceId)
    {
        lock (_sync)
        {
            var updated = GetUnsafe(deviceId).MarkUnavailable();
            _snapshots[deviceId] = updated;
            return updated;
        }
    }

    private DeviceTelemetrySnapshots GetUnsafe(DeviceId deviceId) =>
        _snapshots.TryGetValue(deviceId, out var snapshots)
            ? snapshots
            : DeviceTelemetrySnapshots.Empty(deviceId);
}
