using TR2.Domain;

namespace TR2.Application;

public sealed class CommandCoordinatorRegistry
{
    private readonly Dictionary<DeviceId, CommandCoordinator> _coordinators = [];

    public IReadOnlyCollection<CommandCoordinator> Coordinators => _coordinators.Values;

    public void Register(CommandCoordinator coordinator)
    {
        ArgumentNullException.ThrowIfNull(coordinator);

        if (!_coordinators.TryAdd(coordinator.DeviceId, coordinator))
        {
            throw new InvalidOperationException("A CommandCoordinator is already registered for this device_id.");
        }
    }

    public bool TryGet(DeviceId deviceId, out CommandCoordinator? coordinator) =>
        _coordinators.TryGetValue(deviceId, out coordinator);

    public CommandCoordinator Get(DeviceId deviceId) =>
        _coordinators.TryGetValue(deviceId, out var coordinator)
            ? coordinator
            : throw new KeyNotFoundException("No CommandCoordinator is registered for this device_id.");
}
