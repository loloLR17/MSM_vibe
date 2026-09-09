namespace TR2.Domain;

public sealed record MeasurementPointId
{
    public MeasurementPointId(string value)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(value);
        Value = value;
    }

    public string Value { get; }
}
