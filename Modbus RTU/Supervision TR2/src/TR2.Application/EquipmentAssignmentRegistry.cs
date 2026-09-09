using TR2.Domain;

namespace TR2.Application;

public sealed class EquipmentAssignmentRegistry
{
    private readonly Dictionary<DeviceId, EquipmentAssignment> _activeByDevice = [];
    private readonly Dictionary<MeasurementPointId, EquipmentAssignment> _activeByPoint = [];
    private readonly List<EquipmentAssignment> _history = [];

    public IReadOnlyList<EquipmentAssignment> History => _history;

    public EquipmentAssignment Assign(
        DeviceId deviceId,
        MeasurementPointId measurementPointId,
        DateTimeOffset validFrom)
    {
        ArgumentNullException.ThrowIfNull(measurementPointId);

        if (_activeByDevice.ContainsKey(deviceId))
        {
            throw new InvalidOperationException("The device already has an active measurement-point assignment.");
        }

        if (_activeByPoint.ContainsKey(measurementPointId))
        {
            throw new InvalidOperationException("The measurement point already has an active TR2 assignment.");
        }

        var assignment = new EquipmentAssignment(deviceId, measurementPointId, validFrom);
        _activeByDevice.Add(deviceId, assignment);
        _activeByPoint.Add(measurementPointId, assignment);
        _history.Add(assignment);
        return assignment;
    }

    public EquipmentAssignment Move(
        DeviceId deviceId,
        MeasurementPointId newMeasurementPointId,
        DateTimeOffset movedAt)
    {
        ArgumentNullException.ThrowIfNull(newMeasurementPointId);

        if (!_activeByDevice.TryGetValue(deviceId, out var current))
        {
            throw new InvalidOperationException("The device has no active assignment to move.");
        }

        if (_activeByPoint.ContainsKey(newMeasurementPointId))
        {
            throw new InvalidOperationException("The destination measurement point already has an active TR2 assignment.");
        }

        var closed = current.Close(movedAt);
        ReplaceHistoryEntry(current, closed);
        _activeByPoint.Remove(current.MeasurementPointId);

        var replacement = new EquipmentAssignment(deviceId, newMeasurementPointId, movedAt);
        _activeByDevice[deviceId] = replacement;
        _activeByPoint.Add(newMeasurementPointId, replacement);
        _history.Add(replacement);
        return replacement;
    }

    public EquipmentAssignment Unassign(DeviceId deviceId, DateTimeOffset validTo)
    {
        if (!_activeByDevice.TryGetValue(deviceId, out var current))
        {
            throw new InvalidOperationException("The device has no active assignment to close.");
        }

        var closed = current.Close(validTo);
        ReplaceHistoryEntry(current, closed);
        _activeByDevice.Remove(deviceId);
        _activeByPoint.Remove(current.MeasurementPointId);
        return closed;
    }

    public EquipmentAssignment? GetActiveForDevice(DeviceId deviceId) =>
        _activeByDevice.GetValueOrDefault(deviceId);

    public EquipmentAssignment? GetActiveForPoint(MeasurementPointId measurementPointId)
    {
        ArgumentNullException.ThrowIfNull(measurementPointId);
        return _activeByPoint.GetValueOrDefault(measurementPointId);
    }

    private void ReplaceHistoryEntry(EquipmentAssignment current, EquipmentAssignment closed)
    {
        var index = _history.FindIndex(item => ReferenceEquals(item, current));
        if (index < 0)
        {
            throw new InvalidOperationException("The active assignment is missing from assignment history.");
        }

        _history[index] = closed;
    }
}
