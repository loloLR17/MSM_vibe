using TR2.Domain;

namespace TR2.Application;

public sealed class B3ArchivePublisher
{
    private readonly FleetRegistry _fleet;
    private readonly IB3ArchiveSink _sink;

    public B3ArchivePublisher(FleetRegistry fleet, IB3ArchiveSink sink)
    {
        ArgumentNullException.ThrowIfNull(fleet);
        ArgumentNullException.ThrowIfNull(sink);

        _fleet = fleet;
        _sink = sink;
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

        var observation = new B3ArchiveObservation(
            session.Device.DeviceId,
            readSet.B3,
            receivedAt);

        await _sink.AppendAsync(observation, cancellationToken);
        return observation;
    }
}
