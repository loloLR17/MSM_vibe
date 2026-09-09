namespace TR2.Domain;

public sealed record EquipmentAssignment
{
    public EquipmentAssignment(
        DeviceId deviceId,
        MeasurementPointId measurementPointId,
        DateTimeOffset validFrom,
        DateTimeOffset? validTo = null)
    {
        ArgumentNullException.ThrowIfNull(measurementPointId);

        if (validTo is not null && validTo <= validFrom)
        {
            throw new ArgumentOutOfRangeException(
                nameof(validTo),
                "valid_to must be later than valid_from.");
        }

        DeviceId = deviceId;
        MeasurementPointId = measurementPointId;
        ValidFrom = validFrom;
        ValidTo = validTo;
    }

    public DeviceId DeviceId { get; }

    public MeasurementPointId MeasurementPointId { get; }

    public DateTimeOffset ValidFrom { get; }

    public DateTimeOffset? ValidTo { get; }

    public bool IsActive => ValidTo is null;

    public EquipmentAssignment Close(DateTimeOffset validTo) =>
        new(DeviceId, MeasurementPointId, ValidFrom, validTo);
}
