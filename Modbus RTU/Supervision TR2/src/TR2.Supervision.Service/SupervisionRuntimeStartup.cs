using TR2.Application;

namespace TR2.Supervision.Service;

public sealed class SupervisionRuntimeStartup
{
    private readonly SupervisionRuntimeComposition _composition;
    private bool _started;

    public SupervisionRuntimeStartup(SupervisionRuntimeComposition composition)
    {
        _composition = composition ?? throw new ArgumentNullException(nameof(composition));
    }

    public async ValueTask StartAsync(CancellationToken cancellationToken = default)
    {
        if (_started)
        {
            throw new InvalidOperationException("Supervision runtime startup is one-shot.");
        }

        _started = true;
        cancellationToken.ThrowIfCancellationRequested();

        using (var connection = _composition.Database.OpenConnection())
        {
            // Opening the database applies and verifies the S2 schema/migrations.
        }

        var recoveryService = new CommandCoordinatorRecoveryService(_composition.CommandJournal);
        var deviceIds = await _composition.CommandJournal.ReadDeviceIdsAsync(cancellationToken);

        foreach (var deviceId in deviceIds)
        {
            var coordinator = new CommandCoordinator(
                deviceId,
                _composition.CommandReservationStore,
                _composition.CommandJournal);

            _composition.CommandCoordinatorRegistry.Register(coordinator);
            await recoveryService.RestoreAsync(coordinator, cancellationToken);
        }

        foreach (var bus in _composition.Configuration.Buses)
        {
            if (bus.Serial is null)
            {
                continue;
            }

            await _composition.BusConnectionManager.OpenAsync(
                bus.Bus.Id,
                RuntimeSerialTransportMapper.Map(bus.Serial),
                cancellationToken);
        }

        _composition.ReadinessGate.MarkReady();
    }
}
