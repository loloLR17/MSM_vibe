namespace TR2.Domain;

public sealed record SerialBus
{
    public SerialBus(string id)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(id);
        Id = id;
    }

    public string Id { get; }
}
