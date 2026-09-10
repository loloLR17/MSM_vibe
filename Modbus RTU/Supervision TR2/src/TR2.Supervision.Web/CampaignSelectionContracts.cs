namespace TR2.Supervision.Web;

public sealed record QueueCampaignSelectionRequest(ushort CampaignIndex);

public enum IhmCampaignSelectionQueueStatus
{
    Accepted,
    DeviceNotFound,
    NotReady,
    Conflict
}

public sealed record IhmCampaignSelectionQueueResult(
    IhmCampaignSelectionQueueStatus Status,
    long? WorkId = null,
    uint? DeviceId = null,
    ushort? CampaignIndex = null,
    string? Detail = null);

public interface ISupervisionCampaignSelectionSink
{
    ValueTask<IhmCampaignSelectionQueueResult> QueueCampaignSelectionAsync(
        uint deviceId,
        ushort campaignIndex,
        DateTimeOffset requestedAt,
        CancellationToken cancellationToken = default);
}

public sealed record QueueCampaignSelectionAcceptedResponse(
    DateTimeOffset AcceptedAt,
    long WorkId,
    uint DeviceId,
    ushort CampaignIndex);

public sealed record QueueCampaignSelectionRejectedResponse(
    string Code,
    string Message);
