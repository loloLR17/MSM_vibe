using TR2.Domain;

namespace TR2.Application;

public sealed class B3ArchivePublisher
{
    private readonly FleetRegistry _fleet;
    private readonly IB3ArchiveSink _sink;
    private readonly DeviceTelemetrySnapshotRegistry? _telemetry;

    public B3ArchivePublisher(
        FleetRegistry fleet,
        IB3ArchiveSink sink,
        DeviceTelemetrySnapshotRegistry? telemetry = null)
    {
        ArgumentNullException.ThrowIfNull(fleet);
        ArgumentNullException.ThrowIfNull(sink);

        _fleet = fleet;
        _sink = sink;
        _telemetry = telemetry;
    }

    public async ValueTask<B3ArchiveObservation?> ArchiveAsync(
        TR2Endpoint endpoint,
        PollingReadSet readSet,
        DateTimeOffset receivedAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(endpoint);
        ArgumentNullException.ThrowIfNull(readSet);

        if (readSet.B3 is null)
        {
            return null;
        }

        var session = _fleet.GetSession(endpoint);
        if (session.State != TR2SessionState.Compatible || session.Device is null)
        {
            throw new InvalidOperationException(
                "B3 archiving requires a compatible identified TR2 session.");
        }

        B3ArchiveTimeContext? timeContext = null;
        if (_telemetry is not null)
        {
            var timeState = _telemetry.Get(session.Device.DeviceId).TimeState;
            if (timeState.HasValue && timeState.LastValue is not null && timeState.ReceivedAt is not null)
            {
                timeContext = new B3ArchiveTimeContext(
                    timeState.LastValue,
                    timeState.ReceivedAt.Value);
            }
        }

        var observation = new B3ArchiveObservation(
            session.Device.DeviceId,
            readSet.B3,
            receivedAt,
            timeContext);

        await _sink.AppendAsync(observation, cancellationToken);
        return observation;
    }
}
