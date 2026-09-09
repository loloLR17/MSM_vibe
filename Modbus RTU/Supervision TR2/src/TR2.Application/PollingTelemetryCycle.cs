namespace TR2.Application;

public sealed class PollingTelemetryCycle
{
    private readonly PollingBusOrchestrator _polling;
    private readonly PollingTelemetryPublisher _publisher;
    private readonly IPollingFailureClassifier _failureClassifier;
    private readonly FleetRegistry _fleet;
    private readonly B3ArchivePublisher? _archivePublisher;

    public PollingTelemetryCycle(
        PollingBusOrchestrator polling,
        PollingTelemetryPublisher publisher,
        IPollingFailureClassifier failureClassifier,
        FleetRegistry fleet,
        B3ArchivePublisher? archivePublisher = null)
    {
        ArgumentNullException.ThrowIfNull(polling);
        ArgumentNullException.ThrowIfNull(publisher);
        ArgumentNullException.ThrowIfNull(failureClassifier);
        ArgumentNullException.ThrowIfNull(fleet);
        _polling = polling;
        _publisher = publisher;
        _failureClassifier = failureClassifier;
        _fleet = fleet;
        _archivePublisher = archivePublisher;
    }

    public async ValueTask<DeviceTelemetrySnapshots?> ExecuteAsync(
        ScheduledBusWork work,
        DateTimeOffset receivedAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(work);

        PollingReadSet readSet;
        try
        {
            readSet = await _polling.ExecuteAsync(work, cancellationToken);
        }
        catch (Exception exception) when (_failureClassifier.IsCommunicationFailure(exception))
        {
            _publisher.MarkUnavailable(work.Endpoint);

            var current = _fleet.GetSession(work.Endpoint);
            _fleet.SetSession(current.MarkDisconnected());

            throw;
        }

        var published = _publisher.Publish(work.Endpoint, readSet, receivedAt);

        if (_archivePublisher is not null)
        {
            await _archivePublisher.ArchiveAsync(
                work.Endpoint,
                readSet,
                receivedAt,
                cancellationToken);
        }

        return published;
    }
}
