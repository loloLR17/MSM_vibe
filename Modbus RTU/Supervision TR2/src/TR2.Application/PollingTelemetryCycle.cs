namespace TR2.Application;

public sealed class PollingTelemetryCycle
{
    private readonly PollingBusOrchestrator _polling;
    private readonly PollingTelemetryPublisher _publisher;
    private readonly IPollingFailureClassifier _failureClassifier;

    public PollingTelemetryCycle(
        PollingBusOrchestrator polling,
        PollingTelemetryPublisher publisher,
        IPollingFailureClassifier failureClassifier)
    {
        ArgumentNullException.ThrowIfNull(polling);
        ArgumentNullException.ThrowIfNull(publisher);
        ArgumentNullException.ThrowIfNull(failureClassifier);
        _polling = polling;
        _publisher = publisher;
        _failureClassifier = failureClassifier;
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
            throw;
        }
    }
}
