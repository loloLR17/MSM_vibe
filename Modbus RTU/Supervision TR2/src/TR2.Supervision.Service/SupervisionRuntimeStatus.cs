using TR2.Application;
using TR2.Domain;

namespace TR2.Supervision.Service;

public sealed record SupervisionRuntimeStatus(
    bool IsReady,
    int EndpointCount,
    int CompatibleEndpointCount,
    int DisconnectedEndpointCount,
    int CoordinatorCount,
    int AmbiguousCommandCount);

public sealed class SupervisionRuntimeStatusReader
{
    private readonly SupervisionRuntimeComposition _composition;

    public SupervisionRuntimeStatusReader(SupervisionRuntimeComposition composition)
    {
        _composition = composition ?? throw new ArgumentNullException(nameof(composition));
    }

    public SupervisionRuntimeStatus Read()
    {
        var sessions = _composition.FleetRegistry.Sessions;
        var coordinators = _composition.CommandCoordinatorRegistry.Coordinators;

        return new SupervisionRuntimeStatus(
            _composition.ReadinessGate.IsReady,
            sessions.Count,
            sessions.Count(session => session.State == TR2SessionState.Compatible),
            sessions.Count(session => session.State == TR2SessionState.Disconnected),
            coordinators.Count,
            coordinators.Count(coordinator =>
                coordinator.ActiveTransaction?.State == CommandTransactionState.Ambiguous));
    }
}
