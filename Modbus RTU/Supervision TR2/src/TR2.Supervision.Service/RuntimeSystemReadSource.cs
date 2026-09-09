using TR2.Supervision.Web;

namespace TR2.Supervision.Service;

public sealed class RuntimeSystemReadSource : ISupervisionSystemReadSource
{
    private const int RecentCommunicationFailureLimit = 20;
    private readonly PhysicalSupervisionRuntime _runtime;

    public RuntimeSystemReadSource(PhysicalSupervisionRuntime runtime)
    {
        _runtime = runtime ?? throw new ArgumentNullException(nameof(runtime));
    }

    public IhmSystemReadModel Read()
    {
        var snapshot = RuntimeDiagnosticSnapshot.Capture(_runtime);
        var endpoints = snapshot.Endpoints
            .Select(endpoint => new IhmRuntimeEndpointReadModel(
                endpoint.BusId,
                endpoint.Address,
                endpoint.SessionState.ToString(),
                endpoint.DeviceId))
            .ToArray();

        var failures = _runtime.Composition.CommunicationJournalSink
            .ReadLatest(RecentCommunicationFailureLimit)
            .Select(failure => new IhmCommunicationFailureReadModel(
                failure.FailureId,
                failure.BusId,
                failure.ModbusAddress,
                failure.DeviceId,
                failure.Operation.ToString(),
                failure.Category.ToString(),
                failure.ObservedAt,
                failure.ExceptionType,
                failure.Message))
            .ToArray();

        return new IhmSystemReadModel(
            snapshot.HostState.ToString(),
            snapshot.IsReady,
            snapshot.ConnectedBusIds,
            endpoints,
            failures);
    }
}
