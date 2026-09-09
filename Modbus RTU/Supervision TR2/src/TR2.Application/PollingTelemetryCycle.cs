namespace TR2.Application;

public sealed class PollingTelemetryCycle
{
    private readonly PollingBusOrchestrator _polling;
    private readonly PollingTelemetryPublisher _publisher;
    private readonly IPollingFailureClassifier _failureClassifier;
    private readonly FleetRegistry _fleet;

    public PollingTelemetryCycle(
        PollingBusOrchestrator polling,
        PollingTelemetryPublisher publisher,
        IPollingFailureClassifier failureClassifier,
        FleetRegistry fleet)
    {
        ArgumentNullException.ThrowIfNull(polling);
        ArgumentNullException.ThrowIfNull(publisher);
        ArgumentNullException.ThrowIfNull(failureClassifier);
        ArgumentNullException.ThrowIfNull(fleet);
        _polling = polling;
        _publisher = publisher;
        _failureClassifier = failureClassifier;
        _fleet = fleet;
    }

    public async ValueTask<DeviceTelemetrySnapshots?> ExecuteAsync(
        ScheduledBusWork work,
        DateTimeOffset receivedAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(work);

        try
        {
            var readSet = await _polling.ExecuteAsync(work, cancellationToken);
            return _publisher.Publish(work.Endpoint, readSet, receivedAt);
        }
        catch (Exception exception) when (_failureClassifier.IsCommunicationFailure(exception))
        {
            _publisher.MarkUnavailable(work.Endpoint);

            var current = _fleet.GetSession(work.Endpoint);
            _fleet.SetSession(current.MarkDisconnected());

            throw;
        }
    }
}
