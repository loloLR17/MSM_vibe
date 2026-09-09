namespace TR2.Application;

public sealed class ExplicitRefreshTelemetryCycle
{
    private readonly ExplicitRefreshExecutor _executor;
    private readonly PollingTelemetryPublisher _publisher;
    private readonly B3ArchivePublisher _archive;
    private readonly IPollingFailureClassifier _failureClassifier;
    private readonly FleetRegistry _fleet;
    private readonly CommunicationJournal? _communicationJournal;

    public ExplicitRefreshTelemetryCycle(
        ExplicitRefreshExecutor executor,
        PollingTelemetryPublisher publisher,
        B3ArchivePublisher archive,
        IPollingFailureClassifier failureClassifier,
        FleetRegistry fleet,
        CommunicationJournal? communicationJournal = null)
    {
        ArgumentNullException.ThrowIfNull(executor);
        ArgumentNullException.ThrowIfNull(publisher);
        ArgumentNullException.ThrowIfNull(archive);
        ArgumentNullException.ThrowIfNull(failureClassifier);
        ArgumentNullException.ThrowIfNull(fleet);

        _executor = executor;
        _publisher = publisher;
        _archive = archive;
        _failureClassifier = failureClassifier;
        _fleet = fleet;
        _communicationJournal = communicationJournal;
    }

    public async ValueTask<DeviceTelemetrySnapshots?> ExecuteAndPublishAsync(
        ScheduledBlockRefresh refresh,
        DateTimeOffset receivedAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(refresh);

        PollingReadSet readSet;
        try
        {
            readSet = await _executor.ExecuteAsync(refresh, cancellationToken);
        }
        catch (Exception exception) when (_failureClassifier.IsCommunicationFailure(exception))
        {
            _publisher.MarkUnavailable(refresh.Work.Endpoint);

            var current = _fleet.GetSession(refresh.Work.Endpoint);
            _fleet.SetSession(current.MarkDisconnected());

            if (_communicationJournal is not null)
            {
                try
                {
                    await _communicationJournal.RecordFailureAsync(
                        refresh.Work.Endpoint,
                        CommunicationOperation.ExplicitRefresh,
                        receivedAt,
                        exception,
                        cancellationToken);
                }
                catch
                {
                    // Communication state and the original transport failure remain authoritative.
                }
            }

            throw;
        }

        var published = _publisher.Publish(refresh.Work.Endpoint, readSet, receivedAt);
        await _archive.ArchiveAsync(
            refresh.Work.Endpoint,
            readSet,
            receivedAt,
            cancellationToken);

        return published;
    }
}
