namespace TR2.Application;

public sealed record IhmCampaignInventoryReadModel(
    ushort InventoryStructureVersion,
    ushort TotalCampaignCount,
    ushort ValidCampaignCount,
    ushort SelectedCampaignIndex,
    ushort SelectedCampaignValid,
    uint StorageUsedMb,
    uint StorageFreeMb,
    ushort StorageHealthStatus,
    uint CampaignId,
    uint MissionId,
    uint StartTimestampSeconds,
    uint EndTimestampSeconds,
    ushort CampaignState,
    uint DurationSeconds,
    uint DataSizeMb,
    string CampaignLabel,
    string MissionLabel,
    ushort DataIntegrityStatus);
