using TR2.Domain;

namespace TR2.Application;

public enum CommunicationOperation
{
    Polling,
    ExplicitRefresh,
    CommandTransaction,
    CampaignSelection
}

public enum CommunicationFailureCategory
{
    Unclassified,
    Timeout,
    Io,
    ModbusExceptionResponse
}

public sealed record CommunicationFailureEvent(
    TR2Endpoint Endpoint,
    DeviceId? DeviceId,
    CommunicationOperation Operation,
    CommunicationFailureCategory Category,
    DateTimeOffset ObservedAt,
    string ExceptionType,
    string Message);

public interface ICommunicationJournalSink
{
    ValueTask AppendAsync(
        CommunicationFailureEvent failure,
        CancellationToken cancellationToken = default);
}

public sealed class CommunicationJournal
{
    private readonly FleetRegistry _fleet;
    private readonly ICommunicationJournalSink _sink;

    public CommunicationJournal(FleetRegistry fleet, ICommunicationJournalSink sink)
    {
        ArgumentNullException.ThrowIfNull(fleet);
        ArgumentNullException.ThrowIfNull(sink);
        _fleet = fleet;
        _sink = sink;
    }

    public ValueTask RecordFailureAsync(
        TR2Endpoint endpoint,
        CommunicationOperation operation,
        DateTimeOffset observedAt,
        Exception exception,
        CancellationToken cancellationToken = default) =>
        RecordFailureAsync(
            endpoint,
            operation,
            CommunicationFailureCategory.Unclassified,
            observedAt,
            exception,
            cancellationToken);

    public ValueTask RecordFailureAsync(
        TR2Endpoint endpoint,
        CommunicationOperation operation,
        CommunicationFailureCategory category,
        DateTimeOffset observedAt,
        Exception exception,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(endpoint);
        ArgumentNullException.ThrowIfNull(exception);

        var session = _fleet.GetSession(endpoint);
        var failure = new CommunicationFailureEvent(
            endpoint,
            session.Device?.DeviceId,
            operation,
            category,
            observedAt,
            exception.GetType().FullName ?? exception.GetType().Name,
            exception.Message);

        return _sink.AppendAsync(failure, cancellationToken);
    }
}
