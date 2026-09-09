namespace TR2.Domain;

public sealed record Installation
{
    public Installation(InstallationId id, string name)
    {
        ArgumentNullException.ThrowIfNull(id);
        ArgumentException.ThrowIfNullOrWhiteSpace(name);
        Id = id;
        Name = name;
    }

    public InstallationId Id { get; }

    public string Name { get; }
}
