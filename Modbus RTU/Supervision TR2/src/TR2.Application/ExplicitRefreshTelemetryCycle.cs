namespace TR2.Application;

public sealed class ExplicitRefreshTelemetryCycle
{
    private readonly ExplicitRefreshExecutor _executor;
    private readonly PollingTelemetryPublisher _publisher;
    private readonly IPollingFailureClassifier _failureClassifier;
    private readonly FleetRegistry _fleet;

    public ExplicitRefreshTelemetryCycle(
        ExplicitRefreshExecutor executor,
        PollingTelemetryPublisher publisher,
        IPollingFailureClassifier failureClassifier,
        FleetRegistry fleet)
    {
        ArgumentNullException.ThrowIfNull(executor);
        ArgumentNullException.ThrowIfNull(publisher);
        ArgumentNullException.ThrowIfNull(failureClassifier);
        ArgumentNullException.ThrowIfNull(fleet);

        _executor = executor;
        _publisher = publisher;
        _failureClassifier = failureClassifier;
        _fleet = fleet;
    }

    public async ValueTask<DeviceTelemetrySnapshots?> ExecuteAndPublishAsync(
        ScheduledBlockRefresh refresh,
        DateTimeOffset receivedAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(refresh);

        try
        {
            var readSet = await _executor.ExecuteAsync(refresh, cancellationToken);
            return _publisher.Publish(refresh.Work.Endpoint, readSet, receivedAt);
        }
        catch (Exception exception) when (_failureClassifier.IsCommunicationFailure(exception))
        {
            _publisher.MarkUnavailable(refresh.Work.Endpoint);

            var current = _fleet.GetSession(refresh.Work.Endpoint);
            _fleet.SetSession(current.MarkDisconnected());

            throw;
        }
    }
}
