namespace TR2.Domain;

public sealed record Equipment
{
    public Equipment(EquipmentId id, InstallationId installationId, string name)
    {
        ArgumentNullException.ThrowIfNull(id);
        ArgumentNullException.ThrowIfNull(installationId);
        ArgumentException.ThrowIfNullOrWhiteSpace(name);
        Id = id;
        InstallationId = installationId;
        Name = name;
    }

    public EquipmentId Id { get; }

    public InstallationId InstallationId { get; }

    public string Name { get; }
}
