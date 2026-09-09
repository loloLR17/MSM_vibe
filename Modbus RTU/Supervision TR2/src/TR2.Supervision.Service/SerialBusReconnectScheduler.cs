using TR2.Domain;

namespace TR2.Supervision.Service;

public sealed class SerialBusReconnectScheduler
{
    private readonly SupervisionRuntimeComposition _composition;
    private readonly SerialBusRecoveryCoordinator _recovery;
    private readonly TimeSpan _reconnectInterval;
    private readonly Dictionary<SerialBus, DateTimeOffset> _nextAttempts = [];

    public SerialBusReconnectScheduler(
        SupervisionRuntimeComposition composition,
        TimeSpan reconnectInterval)
    {
        _composition = composition ?? throw new ArgumentNullException(nameof(composition));

        if (reconnectInterval <= TimeSpan.Zero)
        {
            throw new ArgumentOutOfRangeException(nameof(reconnectInterval));
        }

        _recovery = new SerialBusRecoveryCoordinator(composition);
        _reconnectInterval = reconnectInterval;
    }

    public async ValueTask RunDueAsync(
        DateTimeOffset observedAt,
        CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        foreach (var configuredBus in _composition.Configuration.Buses)
        {
            cancellationToken.ThrowIfCancellationRequested();

            if (configuredBus.Serial is null)
            {
                continue;
            }

            if (_composition.BusConnectionManager.TryGet(configuredBus.Bus.Id, out _))
            {
                _nextAttempts.Remove(configuredBus.Bus);
                continue;
            }

            if (!_nextAttempts.TryGetValue(configuredBus.Bus, out var dueAt))
            {
                _nextAttempts.Add(configuredBus.Bus, observedAt + _reconnectInterval);
                continue;
            }

            if (observedAt < dueAt)
            {
                continue;
            }

            try
            {
                var reconnected = await _recovery
                    .TryReconnectAsync(configuredBus.Bus, cancellationToken)
                    .ConfigureAwait(false);

                if (reconnected)
                {
                    _nextAttempts.Remove(configuredBus.Bus);
                }
                else
                {
                    _nextAttempts[configuredBus.Bus] = observedAt + _reconnectInterval;
                }
            }
            catch (OperationCanceledException) when (cancellationToken.IsCancellationRequested)
            {
                throw;
            }
            catch (Exception)
            {
                _nextAttempts[configuredBus.Bus] = observedAt + _reconnectInterval;
            }
        }
    }

    public bool TryGetNextAttempt(SerialBus bus, out DateTimeOffset nextAttempt)
    {
        ArgumentNullException.ThrowIfNull(bus);
        return _nextAttempts.TryGetValue(bus, out nextAttempt);
    }
}
