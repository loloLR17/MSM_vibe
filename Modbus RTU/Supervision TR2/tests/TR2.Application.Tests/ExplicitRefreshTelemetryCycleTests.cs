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
        var cycle = new ExplicitRefreshTelemetryCycle(
            new ExplicitRefreshExecutor(scheduler, new ZeroTransport()),
            new PollingTelemetryPublisher(fleet, telemetry));
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
        var cycle = new ExplicitRefreshTelemetryCycle(
            new ExplicitRefreshExecutor(scheduler, new ZeroTransport()),
            new PollingTelemetryPublisher(fleet, telemetry));
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
        var cycle = new ExplicitRefreshTelemetryCycle(
            new ExplicitRefreshExecutor(scheduler, new ZeroTransport()),
            new PollingTelemetryPublisher(fleet, telemetry));
        var refresh = ActiveRefresh(scheduler, endpoint, block);

        var published = await cycle.ExecuteAndPublishAsync(refresh, ReceivedAt);

        Assert.Null(published);
        var snapshots = telemetry.Get(deviceId);
        Assert.False(snapshots.SystemState.HasValue);
        Assert.False(snapshots.TimeState.HasValue);
        Assert.False(snapshots.VibrationState.HasValue);
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
}
