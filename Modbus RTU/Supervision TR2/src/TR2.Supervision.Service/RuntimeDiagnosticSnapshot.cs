using TR2.Domain;

namespace TR2.Supervision.Service;

public sealed record RuntimeEndpointDiagnostic(
    string BusId,
    byte Address,
    TR2SessionState SessionState,
    uint? DeviceId);

public sealed record RuntimeDiagnosticSnapshot(
    SupervisionRuntimeState HostState,
    bool IsReady,
    IReadOnlyList<string> ConnectedBusIds,
    IReadOnlyList<RuntimeEndpointDiagnostic> Endpoints)
{
    public static RuntimeDiagnosticSnapshot Capture(PhysicalSupervisionRuntime runtime)
    {
        ArgumentNullException.ThrowIfNull(runtime);

        var connectedBusIds = runtime.Composition.BusConnectionManager.ConnectedBusIds
            .OrderBy(busId => busId, StringComparer.Ordinal)
            .ToArray();

        var endpoints = runtime.Composition.FleetRegistry.Sessions
            .Select(session => new RuntimeEndpointDiagnostic(
                session.Endpoint.Bus.Id,
                session.Endpoint.Address.Value,
                session.State,
                session.Device?.DeviceId.Value))
            .OrderBy(endpoint => endpoint.BusId, StringComparer.Ordinal)
            .ThenBy(endpoint => endpoint.Address)
            .ToArray();

        return new RuntimeDiagnosticSnapshot(
            runtime.Host.State,
            runtime.Composition.ReadinessGate.IsReady,
            connectedBusIds,
            endpoints);
    }
}
