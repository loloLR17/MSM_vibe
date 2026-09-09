using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Application.Tests;

public sealed class PollingB3ArchiveIntegrationTests
{
    private static readonly DateTimeOffset Now =
        new(2026, 9, 9, 16, 0, 0, TimeSpan.Zero);

    [Fact]
    public async Task Fast_polling_archives_B3_after_successful_read_and_snapshot_publication()
    {
        var endpoint = Endpoint();
        var deviceId = new DeviceId(1001);
        var fleet = CompatibleFleet(endpoint, deviceId);
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var sink = new RecordingArchiveSink();
        var scheduler = new BusWorkScheduler();
        var polling = new PollingBusOrchestrator(
            scheduler,
            new PollingExecutor(new ZeroTransport(), supportedProtocolVersion: 1));
        var cycle = new PollingTelemetryCycle(
            polling,
            new PollingTelemetryPublisher(fleet, telemetry),
            new IOExceptionFailureClassifier(),
            fleet,
            new B3ArchivePublisher(fleet, sink));

        polling.Queue(endpoint, PollingGroup.Fast, Now);
        var active = polling.BeginNext(endpoint.Bus, Now)!;

        var published = await cycle.ExecuteAsync(active, Now);

        Assert.NotNull(published);
        Assert.Single(sink.Observations);
        var archived = sink.Observations[0];
        Assert.Equal(deviceId, archived.DeviceId);
        Assert.Equal(Now, archived.ReceivedAt);
        Assert.Equal(published!.VibrationState.LastValue, archived.Value);
    }

    [Theory]
    [InlineData(PollingGroup.Medium)]
    [InlineData(PollingGroup.Slow)]
    [InlineData(PollingGroup.Static)]
    public async Task Polling_without_B3_does_not_archive(
        PollingGroup group)
    {
        var endpoint = Endpoint();
        var fleet = CompatibleFleet(endpoint, new DeviceId(1001));
        var sink = new RecordingArchiveSink();
        var scheduler = new BusWorkScheduler();
        var polling = new PollingBusOrchestrator(
            scheduler,
            new PollingExecutor(new ZeroTransport(), supportedProtocolVersion: 1));
        var cycle = new PollingTelemetryCycle(
            polling,
            new PollingTelemetryPublisher(fleet, new DeviceTelemetrySnapshotRegistry()),
            new IOExceptionFailureClassifier(),
            fleet,
            new B3ArchivePublisher(fleet, sink));

        polling.Queue(endpoint, group, Now);
        var active = polling.BeginNext(endpoint.Bus, Now)!;

        await cycle.ExecuteAsync(active, Now);

        Assert.Empty(sink.Observations);
    }

    [Fact]
    public async Task Archive_failure_propagates_without_disconnect_or_snapshot_rollback()
    {
        var endpoint = Endpoint();
        var deviceId = new DeviceId(1001);
        var fleet = CompatibleFleet(endpoint, deviceId);
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var scheduler = new BusWorkScheduler();
        var polling = new PollingBusOrchestrator(
            scheduler,
            new PollingExecutor(new ZeroTransport(), supportedProtocolVersion: 1));
        var cycle = new PollingTelemetryCycle(
            polling,
            new PollingTelemetryPublisher(fleet, telemetry),
            new IOExceptionFailureClassifier(),
            fleet,
            new B3ArchivePublisher(fleet, new FailingArchiveSink()));

        polling.Queue(endpoint, PollingGroup.Fast, Now);
        var active = polling.BeginNext(endpoint.Bus, Now)!;

        await Assert.ThrowsAsync<IOException>(async () =>
            await cycle.ExecuteAsync(active, Now));

        Assert.Equal(TR2SessionState.Compatible, fleet.GetSession(endpoint).State);
        Assert.True(telemetry.Get(deviceId).VibrationState.HasValue);
        Assert.True(telemetry.Get(deviceId).VibrationState.IsAvailable);
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

    private sealed class ZeroTransport : IRegisterTransport
    {
        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromResult(new ushort[registerCount]);
    }

    private sealed class RecordingArchiveSink : IB3ArchiveSink
    {
        public List<B3ArchiveObservation> Observations { get; } = [];

        public ValueTask AppendAsync(
            B3ArchiveObservation observation,
            CancellationToken cancellationToken = default)
        {
            Observations.Add(observation);
            return ValueTask.CompletedTask;
        }
    }

    private sealed class FailingArchiveSink : IB3ArchiveSink
    {
        public ValueTask AppendAsync(
            B3ArchiveObservation observation,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromException(new IOException("Injected archive storage failure."));
    }
}
