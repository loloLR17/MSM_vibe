using TR2.Domain;

namespace TR2.Application;

public sealed class RecoveredCommandReconciliationPlanner
{
    private readonly CommandCoordinatorRegistry _coordinators;
    private readonly CommandBusOrchestrator _orchestrator;

    public RecoveredCommandReconciliationPlanner(
        CommandCoordinatorRegistry coordinators,
        CommandBusOrchestrator orchestrator)
    {
        ArgumentNullException.ThrowIfNull(coordinators);
        ArgumentNullException.ThrowIfNull(orchestrator);
        _coordinators = coordinators;
        _orchestrator = orchestrator;
    }

    public ScheduledBusWork? QueueForSession(TR2Session session, DateTimeOffset dueAt)
    {
        ArgumentNullException.ThrowIfNull(session);

        if (session.State != TR2SessionState.Compatible || session.Device is null)
        {
            return null;
        }

        if (!_coordinators.TryGet(session.Device.DeviceId, out var coordinator)
            || coordinator?.ActiveTransaction?.State != CommandTransactionState.Ambiguous)
        {
            return null;
        }

        return _orchestrator.QueueReconciliation(session.Endpoint, coordinator, dueAt);
    }
}
