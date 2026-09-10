using TR2.Domain;

namespace TR2.Application;

public sealed class CommandCoordinatorRegistry
{
    private readonly object _sync = new();
    private readonly Dictionary<DeviceId, CommandCoordinator> _coordinators = [];

    public IReadOnlyCollection<CommandCoordinator> Coordinators
    {
        get
        {
            lock (_sync)
            {
                return _coordinators.Values.ToArray();
            }
        }
    }

    public void Register(CommandCoordinator coordinator)
    {
        ArgumentNullException.ThrowIfNull(coordinator);

        lock (_sync)
        {
            if (!_coordinators.TryAdd(coordinator.DeviceId, coordinator))
            {
                throw new InvalidOperationException("A CommandCoordinator is already registered for this device_id.");
            }
        }
    }

    public CommandCoordinator GetOrAdd(
        DeviceId deviceId,
        Func<DeviceId, CommandCoordinator> factory)
    {
        ArgumentNullException.ThrowIfNull(factory);

        lock (_sync)
        {
            if (_coordinators.TryGetValue(deviceId, out var existing))
            {
                return existing;
            }

            var coordinator = factory(deviceId)
                ?? throw new InvalidOperationException("The CommandCoordinator factory returned null.");
            if (coordinator.DeviceId != deviceId)
            {
                throw new InvalidOperationException("The CommandCoordinator factory returned a coordinator for another device_id.");
            }

            _coordinators.Add(deviceId, coordinator);
            return coordinator;
        }
    }

    public bool TryGet(DeviceId deviceId, out CommandCoordinator? coordinator)
    {
        lock (_sync)
        {
            return _coordinators.TryGetValue(deviceId, out coordinator);
        }
    }

    public CommandCoordinator Get(DeviceId deviceId)
    {
        lock (_sync)
        {
            return _coordinators.TryGetValue(deviceId, out var coordinator)
                ? coordinator
                : throw new KeyNotFoundException("No CommandCoordinator is registered for this device_id.");
        }
    }
}
