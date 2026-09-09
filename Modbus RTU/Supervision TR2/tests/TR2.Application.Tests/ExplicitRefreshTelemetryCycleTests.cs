using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Application.Tests;

public sealed class ExplicitRefreshTelemetryCycleTests
{
    private static readonly DateTimeOffset DueAt =
        new(2026, 9, 9, 15, 0, 0, TimeSpan.Zero);

    private static readonly DateTimeOffset ReceivedAt = DueAt.AddSeconds(1);

    [Theory]
    [InlineData(TR2RegisterBlock.B1)]
    [InlineData(TR2RegisterBlock.B2)]
    [InlineData(TR2RegisterBlock.B3)]
    public async Task Telemetry_refresh_is_published_to_device_snapshots(
        TR2RegisterBlock block)
    {
        var endpoint = Endpoint();
        var deviceId = new DeviceId(1001);
        var fleet = CompatibleFleet(endpoint, deviceId);
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var scheduler = new BusWorkScheduler();
        var cycle = Cycle(scheduler, new ZeroTransport(), fleet, telemetry);
        var refresh = ActiveRefresh(scheduler, endpoint, block);

        var published = await cycle.ExecuteAndPublishAsync(refresh, ReceivedAt);

        Assert.NotNull(published);
        var snapshots = telemetry.Get(deviceId);

        Assert.Equal(block == TR2RegisterBlock.B1, snapshots.SystemState.HasValue);
        Assert.Equal(block == TR2RegisterBlock.B2, snapshots.TimeState.HasValue);
        Assert.Equal(block == TR2RegisterBlock.B3, snapshots.VibrationState.HasValue);

        if (block == TR2RegisterBlock.B1)
        {
            Assert.Equal(ReceivedAt, snapshots.SystemState.ReceivedAt);
        }
        else if (block == TR2RegisterBlock.B2)
        {
            Assert.Equal(ReceivedAt, snapshots.TimeState.ReceivedAt);
        }
        else
        {
            Assert.Equal(ReceivedAt, snapshots.VibrationState.ReceivedAt);
        }
    }

    [Fact]
    public async Task B3_refresh_is_archived_with_device_identity_and_receive_time()
    {
        var endpoint = Endpoint();
        var deviceId = new DeviceId(1001);
        var fleet = CompatibleFleet(endpoint, deviceId);
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var archiveSink = new RecordingArchiveSink();
        var scheduler = new BusWorkScheduler();
        var cycle = Cycle(scheduler, new ZeroTransport(), fleet, telemetry, archiveSink);
        var refresh = ActiveRefresh(scheduler, endpoint, TR2RegisterBlock.B3);

        await cycle.ExecuteAndPublishAsync(refresh, ReceivedAt);

        var observation = Assert.Single(archiveSink.Observations);
        Assert.Equal(deviceId, observation.DeviceId);
        Assert.Equal(ReceivedAt, observation.ReceivedAt);
        Assert.Equal(telemetry.Get(deviceId).VibrationState.LastValue, observation.Value);
    }

    [Fact]
    public async Task Non_B3_refresh_does_not_archive_observation()
    {
        var endpoint = Endpoint();
        var deviceId = new DeviceId(1001);
        var fleet = CompatibleFleet(endpoint, deviceId);
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var archiveSink = new RecordingArchiveSink();
        var scheduler = new BusWorkScheduler();
        var cycle = Cycle(scheduler, new ZeroTransport(), fleet, telemetry, archiveSink);
        var refresh = ActiveRefresh(scheduler, endpoint, TR2RegisterBlock.B1);

        await cycle.ExecuteAndPublishAsync(refresh, ReceivedAt);

        Assert.Empty(archiveSink.Observations);
    }

    [Fact]
    public async Task Archive_failure_does_not_disconnect_successfully_refreshed_device()
    {
        var endpoint = Endpoint();
        var deviceId = new DeviceId(1001);
        var fleet = CompatibleFleet(endpoint, deviceId);
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var scheduler = new BusWorkScheduler();
        var cycle = Cycle(
            scheduler,
            new ZeroTransport(),
            fleet,
            telemetry,
            new ThrowingArchiveSink(new IOException("Injected archive failure.")));
        var refresh = ActiveRefresh(scheduler, endpoint, TR2RegisterBlock.B3);

        var error = await Assert.ThrowsAsync<IOException>(async () =>
            await cycle.ExecuteAndPublishAsync(refresh, ReceivedAt));

        Assert.Equal("Injected archive failure.", error.Message);
        Assert.Equal(TR2SessionState.Compatible, fleet.GetSession(endpoint).State);
        Assert.True(telemetry.Get(deviceId).VibrationState.IsAvailable);
        Assert.Equal(ReceivedAt, telemetry.Get(deviceId).VibrationState.ReceivedAt);
        Assert.Null(scheduler.BeginNext(endpoint.Bus, ReceivedAt));
    }

    [Fact]
    public async Task Successful_refresh_restores_availability_for_refreshed_telemetry_block()
    {
        var endpoint = Endpoint();
        var deviceId = new DeviceId(1001);
        var fleet = CompatibleFleet(endpoint, deviceId);
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        telemetry.ReceiveSystemState(
            deviceId,
            new TR2.Protocol.B1SystemState(1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
            DueAt.AddMinutes(-1));
        telemetry.MarkUnavailable(deviceId);

        var scheduler = new BusWorkScheduler();
        var cycle = Cycle(scheduler, new ZeroTransport(), fleet, telemetry);
        var refresh = ActiveRefresh(scheduler, endpoint, TR2RegisterBlock.B1);

        await cycle.ExecuteAndPublishAsync(refresh, ReceivedAt);

        var updated = telemetry.Get(deviceId).SystemState;
        Assert.True(updated.IsAvailable);
        Assert.Equal(ReceivedAt, updated.ReceivedAt);
    }

    [Theory]
    [InlineData(TR2RegisterBlock.B4)]
    [InlineData(TR2RegisterBlock.B5)]
    [InlineData(TR2RegisterBlock.B6)]
    [InlineData(TR2RegisterBlock.B7)]
    public async Task Non_telemetry_refresh_does_not_create_telemetry_snapshot(
        TR2RegisterBlock block)
    {
        var endpoint = Endpoint();
        var deviceId = new DeviceId(1001);
        var fleet = CompatibleFleet(endpoint, deviceId);
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var scheduler = new BusWorkScheduler();
        var cycle = Cycle(scheduler, new ZeroTransport(), fleet, telemetry);
        var refresh = ActiveRefresh(scheduler, endpoint, block);

        var published = await cycle.ExecuteAndPublishAsync(refresh, ReceivedAt);

        Assert.Null(published);
        var snapshots = telemetry.Get(deviceId);
        Assert.False(snapshots.SystemState.HasValue);
        Assert.False(snapshots.TimeState.HasValue);
        Assert.False(snapshots.VibrationState.HasValue);
    }

    [Fact]
    public async Task Classified_communication_failure_marks_unavailable_and_disconnects_session()
    {
        var endpoint = Endpoint();
        var deviceId = new DeviceId(1001);
        var fleet = CompatibleFleet(endpoint, deviceId);
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var previous = new TR2.Protocol.B1SystemState(1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        telemetry.ReceiveSystemState(deviceId, previous, DueAt.AddMinutes(-1));

        var scheduler = new BusWorkScheduler();
        var cycle = Cycle(
            scheduler,
            new FailingTransport(new IOException("Injected communication failure.")),
            fleet,
            telemetry);
        var refresh = ActiveRefresh(scheduler, endpoint, TR2RegisterBlock.B1);

        await Assert.ThrowsAsync<IOException>(async () =>
            await cycle.ExecuteAndPublishAsync(refresh, ReceivedAt));

        var snapshots = telemetry.Get(deviceId);
        Assert.Equal(previous, snapshots.SystemState.LastValue);
        Assert.False(snapshots.SystemState.IsAvailable);

        var session = fleet.GetSession(endpoint);
        Assert.Equal(TR2SessionState.Disconnected, session.State);
        Assert.Equal(deviceId, session.Device!.DeviceId);
        Assert.Null(scheduler.BeginNext(endpoint.Bus, ReceivedAt));
    }

    [Fact]
    public async Task Unclassified_failure_does_not_change_availability_or_session_state()
    {
        var endpoint = Endpoint();
        var deviceId = new DeviceId(1001);
        var fleet = CompatibleFleet(endpoint, deviceId);
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        telemetry.ReceiveSystemState(
            deviceId,
            new TR2.Protocol.B1SystemState(1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
            DueAt.AddMinutes(-1));

        var scheduler = new BusWorkScheduler();
        var cycle = Cycle(
            scheduler,
            new FailingTransport(new InvalidOperationException("Injected non-communication failure.")),
            fleet,
            telemetry);
        var refresh = ActiveRefresh(scheduler, endpoint, TR2RegisterBlock.B1);

        await Assert.ThrowsAsync<InvalidOperationException>(async () =>
            await cycle.ExecuteAndPublishAsync(refresh, ReceivedAt));

        Assert.True(telemetry.Get(deviceId).SystemState.IsAvailable);
        Assert.Equal(TR2SessionState.Compatible, fleet.GetSession(endpoint).State);
        Assert.Null(scheduler.BeginNext(endpoint.Bus, ReceivedAt));
    }

    private static ExplicitRefreshTelemetryCycle Cycle(
        BusWorkScheduler scheduler,
        IRegisterTransport transport,
        FleetRegistry fleet,
        DeviceTelemetrySnapshotRegistry telemetry,
        IB3ArchiveSink? archiveSink = null)
    {
        archiveSink ??= new RecordingArchiveSink();

        return new ExplicitRefreshTelemetryCycle(
            new ExplicitRefreshExecutor(scheduler, transport),
            new PollingTelemetryPublisher(fleet, telemetry),
            new B3ArchivePublisher(fleet, archiveSink),
            new IOExceptionFailureClassifier(),
            fleet);
    }

    private static ScheduledBlockRefresh ActiveRefresh(
        BusWorkScheduler scheduler,
        TR2Endpoint endpoint,
        TR2RegisterBlock block)
    {
        scheduler.QueuePriority(endpoint, BusWorkKind.ExplicitRefresh, DueAt);
        var active = scheduler.BeginNext(endpoint.Bus, DueAt)!;
        return new ScheduledBlockRefresh(active, block);
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

    private sealed class ThrowingArchiveSink(Exception exception) : IB3ArchiveSink
    {
        public ValueTask AppendAsync(
            B3ArchiveObservation observation,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromException(exception);
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
}
