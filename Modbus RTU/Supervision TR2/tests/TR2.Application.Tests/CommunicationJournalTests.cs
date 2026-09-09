using TR2.Domain;
using Xunit;

namespace TR2.Application.Tests;

public sealed class CommunicationJournalTests
{
    private static readonly DateTimeOffset ObservedAt =
        new(2026, 9, 9, 16, 30, 0, TimeSpan.Zero);

    [Fact]
    public async Task Known_device_identity_is_recorded_with_structured_failure_context()
    {
        var endpoint = Endpoint();
        var deviceId = new DeviceId(1001);
        var fleet = new FleetRegistry();
        fleet.RegisterEndpoint(endpoint);
        fleet.SetSession(TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));
        var sink = new RecordingSink();
        var journal = new CommunicationJournal(fleet, sink);
        var error = new IOException("Injected communication failure.");

        await journal.RecordFailureAsync(
            endpoint,
            CommunicationOperation.Polling,
            ObservedAt,
            error);

        var recorded = Assert.Single(sink.Failures);
        Assert.Equal(endpoint, recorded.Endpoint);
        Assert.Equal(deviceId, recorded.DeviceId);
        Assert.Equal(CommunicationOperation.Polling, recorded.Operation);
        Assert.Equal(ObservedAt, recorded.ObservedAt);
        Assert.Equal(typeof(IOException).FullName, recorded.ExceptionType);
        Assert.Equal(error.Message, recorded.Message);
    }

    [Fact]
    public async Task Unknown_device_identity_remains_null_instead_of_being_inferred_from_endpoint()
    {
        var endpoint = Endpoint();
        var fleet = new FleetRegistry();
        fleet.RegisterEndpoint(endpoint);
        var sink = new RecordingSink();
        var journal = new CommunicationJournal(fleet, sink);

        await journal.RecordFailureAsync(
            endpoint,
            CommunicationOperation.ExplicitRefresh,
            ObservedAt,
            new IOException("Injected communication failure."));

        var recorded = Assert.Single(sink.Failures);
        Assert.Null(recorded.DeviceId);
        Assert.Equal(CommunicationOperation.ExplicitRefresh, recorded.Operation);
    }

    [Fact]
    public async Task Journal_sink_failure_is_not_hidden_by_the_contract()
    {
        var endpoint = Endpoint();
        var fleet = new FleetRegistry();
        fleet.RegisterEndpoint(endpoint);
        var journal = new CommunicationJournal(fleet, new FailingSink());

        var error = await Assert.ThrowsAsync<InvalidOperationException>(async () =>
            await journal.RecordFailureAsync(
                endpoint,
                CommunicationOperation.Polling,
                ObservedAt,
                new IOException("Injected communication failure.")));

        Assert.Equal("Injected journal failure.", error.Message);
    }

    private static TR2Endpoint Endpoint() =>
        new(new SerialBus("RS485-A"), new ModbusAddress(10));

    private sealed class RecordingSink : ICommunicationJournalSink
    {
        public List<CommunicationFailureEvent> Failures { get; } = [];

        public ValueTask AppendAsync(
            CommunicationFailureEvent failure,
            CancellationToken cancellationToken = default)
        {
            Failures.Add(failure);
            return ValueTask.CompletedTask;
        }
    }

    private sealed class FailingSink : ICommunicationJournalSink
    {
        public ValueTask AppendAsync(
            CommunicationFailureEvent failure,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromException(new InvalidOperationException("Injected journal failure."));
    }
}
