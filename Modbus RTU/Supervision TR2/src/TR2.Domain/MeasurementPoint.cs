namespace TR2.Domain;

public sealed record MeasurementPoint
{
    public MeasurementPoint(MeasurementPointId id, EquipmentId equipmentId, string name)
    {
        ArgumentNullException.ThrowIfNull(id);
        ArgumentNullException.ThrowIfNull(equipmentId);
        ArgumentException.ThrowIfNullOrWhiteSpace(name);
        Id = id;
        EquipmentId = equipmentId;
        Name = name;
    }

    public MeasurementPointId Id { get; }

    public EquipmentId EquipmentId { get; }

    public string Name { get; }
}
