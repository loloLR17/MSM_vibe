using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Application.Tests;

public sealed class CommunicationJournalIntegrationTests
{
    private static readonly DateTimeOffset ObservedAt =
        new(2026, 9, 9, 16, 30, 0, TimeSpan.Zero);

    [Fact]
    public async Task Polling_communication_failure_is_journaled_with_device_identity()
    {
        var endpoint = Endpoint();
        var deviceId = new DeviceId(1001);
        var fleet = CompatibleFleet(endpoint, deviceId);
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var journalSink = new RecordingJournalSink();
        var scheduler = new BusWorkScheduler();
        var polling = new PollingBusOrchestrator(
            scheduler,
            new PollingExecutor(
                new FailingTransport(new IOException("Injected transport failure.")),
                supportedProtocolVersion: 1));
        var cycle = new PollingTelemetryCycle(
            polling,
            new PollingTelemetryPublisher(fleet, telemetry),
            new IOExceptionFailureClassifier(),
            fleet,
            communicationJournal: new CommunicationJournal(fleet, journalSink));

        polling.Queue(endpoint, PollingGroup.Fast, ObservedAt);
        var active = polling.BeginNext(endpoint.Bus, ObservedAt)!;

        await Assert.ThrowsAsync<IOException>(async () =>
            await cycle.ExecuteAsync(active, ObservedAt));

        var failure = Assert.Single(journalSink.Failures);
        Assert.Equal(endpoint, failure.Endpoint);
        Assert.Equal(deviceId, failure.DeviceId);
        Assert.Equal(CommunicationOperation.Polling, failure.Operation);
        Assert.Equal(ObservedAt, failure.ObservedAt);
        Assert.Contains(nameof(IOException), failure.ExceptionType);
        Assert.Equal("Injected transport failure.", failure.Message);
        Assert.Equal(TR2SessionState.Disconnected, fleet.GetSession(endpoint).State);
    }

    [Fact]
    public async Task Explicit_refresh_communication_failure_is_journaled()
    {
        var endpoint = Endpoint();
        var deviceId = new DeviceId(1001);
        var fleet = CompatibleFleet(endpoint, deviceId);
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var journalSink = new RecordingJournalSink();
        var scheduler = new BusWorkScheduler();
        var cycle = new ExplicitRefreshTelemetryCycle(
            new ExplicitRefreshExecutor(
                scheduler,
                new FailingTransport(new IOException("Injected refresh failure."))),
            new PollingTelemetryPublisher(fleet, telemetry),
            new B3ArchivePublisher(fleet, new NoOpArchiveSink()),
            new IOExceptionFailureClassifier(),
            fleet,
            new CommunicationJournal(fleet, journalSink));

        scheduler.QueuePriority(endpoint, BusWorkKind.ExplicitRefresh, ObservedAt);
        var active = scheduler.BeginNext(endpoint.Bus, ObservedAt)!;
        var refresh = new ScheduledBlockRefresh(active, TR2RegisterBlock.B1);

        await Assert.ThrowsAsync<IOException>(async () =>
            await cycle.ExecuteAndPublishAsync(refresh, ObservedAt));

        var failure = Assert.Single(journalSink.Failures);
        Assert.Equal(deviceId, failure.DeviceId);
        Assert.Equal(CommunicationOperation.ExplicitRefresh, failure.Operation);
        Assert.Equal("Injected refresh failure.", failure.Message);
        Assert.Equal(TR2SessionState.Disconnected, fleet.GetSession(endpoint).State);
    }

    [Fact]
    public async Task Journal_failure_does_not_mask_transport_failure_or_disconnect_transition()
    {
        var endpoint = Endpoint();
        var deviceId = new DeviceId(1001);
        var fleet = CompatibleFleet(endpoint, deviceId);
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var scheduler = new BusWorkScheduler();
        var polling = new PollingBusOrchestrator(
            scheduler,
            new PollingExecutor(
                new FailingTransport(new IOException("Original transport failure.")),
                supportedProtocolVersion: 1));
        var cycle = new PollingTelemetryCycle(
            polling,
            new PollingTelemetryPublisher(fleet, telemetry),
            new IOExceptionFailureClassifier(),
            fleet,
            communicationJournal: new CommunicationJournal(fleet, new FailingJournalSink()));

        polling.Queue(endpoint, PollingGroup.Fast, ObservedAt);
        var active = polling.BeginNext(endpoint.Bus, ObservedAt)!;

        var error = await Assert.ThrowsAsync<IOException>(async () =>
            await cycle.ExecuteAsync(active, ObservedAt));

        Assert.Equal("Original transport failure.", error.Message);
        Assert.Equal(TR2SessionState.Disconnected, fleet.GetSession(endpoint).State);
    }

    private static FleetRegistry CompatibleFleet(TR2Endpoint endpoint, DeviceId deviceId)
    {
        var fleet = new FleetRegistry();
        fleet.RegisterEndpoint(endpoint);
        fleet.SetSession(TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));
        return fleet;
    }

    private static TR2Endpoint Endpoint() =>
        new(new SerialBus("RS485-A"), new ModbusAddress(10));

    private sealed class IOExceptionFailureClassifier : IPollingFailureClassifier
    {
        public bool IsCommunicationFailure(Exception exception) => exception is IOException;
    }

    private sealed class FailingTransport(Exception exception) : IRegisterTransport
    {
        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromException<ushort[]>(exception);
    }

    private sealed class RecordingJournalSink : ICommunicationJournalSink
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

    private sealed class FailingJournalSink : ICommunicationJournalSink
    {
        public ValueTask AppendAsync(
            CommunicationFailureEvent failure,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromException(new IOException("Injected journal failure."));
    }

    private sealed class NoOpArchiveSink : IB3ArchiveSink
    {
        public ValueTask AppendAsync(
            B3ArchiveObservation observation,
            CancellationToken cancellationToken = default) =>
            ValueTask.CompletedTask;
    }
}
