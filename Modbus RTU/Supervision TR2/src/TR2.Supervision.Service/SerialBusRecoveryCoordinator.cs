using TR2.Domain;

namespace TR2.Supervision.Service;

public sealed class SerialBusRecoveryCoordinator
{
    private readonly SupervisionRuntimeComposition _composition;

    public SerialBusRecoveryCoordinator(SupervisionRuntimeComposition composition)
    {
        _composition = composition ?? throw new ArgumentNullException(nameof(composition));
    }

    public async ValueTask MarkDisconnectedAsync(
        SerialBus bus,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(bus);
        cancellationToken.ThrowIfCancellationRequested();

        await _composition.BusConnectionManager.CloseAsync(bus.Id).ConfigureAwait(false);

        foreach (var endpoint in GetConfiguredBus(bus).Endpoints)
        {
            _composition.FleetRegistry.SetSession(TR2Session.CreateUnidentified(endpoint));
        }
    }

    public async ValueTask<bool> TryReconnectAsync(
        SerialBus bus,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(bus);
        cancellationToken.ThrowIfCancellationRequested();

        var configuredBus = GetConfiguredBus(bus);
        if (configuredBus.Serial is null)
        {
            return false;
        }

        if (_composition.BusConnectionManager.TryGet(bus.Id, out _))
        {
            return true;
        }

        var settings = RuntimeSerialTransportMapper.Map(configuredBus.Serial);
        await _composition.BusConnectionManager
            .OpenAsync(bus.Id, settings, cancellationToken)
            .ConfigureAwait(false);

        foreach (var endpoint in configuredBus.Endpoints)
        {
            _composition.FleetRegistry.SetSession(TR2Session.CreateUnidentified(endpoint));
        }

        return true;
    }

    private RuntimeBusConfiguration GetConfiguredBus(SerialBus bus)
    {
        var configuredBus = _composition.Configuration.Buses.SingleOrDefault(candidate => candidate.Bus == bus);
        return configuredBus
            ?? throw new KeyNotFoundException($"Bus '{bus.Id}' is not configured.");
    }
}
