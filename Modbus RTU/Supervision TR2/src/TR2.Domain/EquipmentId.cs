namespace TR2.Domain;

public sealed record EquipmentId
{
    public EquipmentId(string value)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(value);
        Value = value;
    }

    public string Value { get; }
}
