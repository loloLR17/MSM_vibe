namespace TR2.Supervision.Web;

public sealed record IhmRuntimeEndpointReadModel(
    string BusId,
    byte ModbusAddress,
    string SessionState,
    uint? DeviceId);

public sealed record IhmCommunicationFailureReadModel(
    long FailureId,
    string BusId,
    byte ModbusAddress,
    uint? DeviceId,
    string Operation,
    string Category,
    DateTimeOffset ObservedAt,
    string ExceptionType,
    string Message);

public sealed record IhmSystemReadModel(
    string HostState,
    bool IsReady,
    IReadOnlyList<string> ConnectedBusIds,
    IReadOnlyList<IhmRuntimeEndpointReadModel> Endpoints,
    IReadOnlyList<IhmCommunicationFailureReadModel> RecentCommunicationFailures);

public interface ISupervisionSystemReadSource
{
    IhmSystemReadModel Read();
}

public sealed record SystemReadResponse(
    DateTimeOffset ObservedAt,
    IhmSystemReadModel System);
