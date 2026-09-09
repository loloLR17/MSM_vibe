namespace TR2.Domain;

public sealed record InstallationId
{
    public InstallationId(string value)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(value);
        Value = value;
    }

    public string Value { get; }
}
