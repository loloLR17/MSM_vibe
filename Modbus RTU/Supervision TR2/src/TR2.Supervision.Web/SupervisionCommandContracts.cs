namespace TR2.Supervision.Web;

public enum IhmB5Command
{
    ApplyConfig,
    SyncTime,
    StartAcquisition,
    StopAcquisition,
    Selftest,
    AcknowledgeFault,
    RefreshIndicators,
    EnterMaintenance,
    ExitMaintenance,
    SoftwareReset
}

public sealed record QueueB5CommandRequest(
    string? RequestIdentity,
    IhmB5Command? Command,
    ushort? FaultCode = null,
    bool? AcknowledgeAll = null,
    bool? ConfirmProtectedCommand = null);

public sealed record IhmB5CommandSubmission(
    IhmB5Command Command,
    ushort? FaultCode = null,
    bool? AcknowledgeAll = null,
    bool ConfirmProtectedCommand = false);

public enum IhmCommandQueueStatus
{
    Accepted,
    DeviceNotFound,
    DuplicateRequestIdentity,
    NotReady,
    Conflict
}

public sealed record IhmCommandQueueResult(
    IhmCommandQueueStatus Status,
    long? WorkId = null,
    uint? DeviceId = null,
    ushort? TransactionId = null,
    string? Detail = null);

public interface ISupervisionCommandSink
{
    ValueTask<IhmCommandQueueResult> QueueAsync(
        uint deviceId,
        string requestIdentity,
        IhmB5CommandSubmission submission,
        DateTimeOffset requestedAt,
        CancellationToken cancellationToken = default);
}

public sealed record QueueB5CommandAcceptedResponse(
    DateTimeOffset AcceptedAt,
    long WorkId,
    uint DeviceId,
    ushort TransactionId,
    IhmB5Command Command,
    string RequestIdentity);

public sealed record QueueB5CommandRejectedResponse(
    string Code,
    string Message);
