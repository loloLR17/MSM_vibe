namespace TR2.Application;

public sealed class ExplicitRefreshTelemetryCycle
{
    private readonly ExplicitRefreshExecutor _executor;
    private readonly PollingTelemetryPublisher _publisher;

    public ExplicitRefreshTelemetryCycle(
        ExplicitRefreshExecutor executor,
        PollingTelemetryPublisher publisher)
    {
        ArgumentNullException.ThrowIfNull(executor);
        ArgumentNullException.ThrowIfNull(publisher);

        _executor = executor;
        _publisher = publisher;
    }

    public async ValueTask<DeviceTelemetrySnapshots?> ExecuteAndPublishAsync(
        ScheduledBlockRefresh refresh,
        DateTimeOffset receivedAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(refresh);

        var readSet = await _executor.ExecuteAsync(refresh, cancellationToken);
        return _publisher.Publish(refresh.Work.Endpoint, readSet, receivedAt);
    }
}
