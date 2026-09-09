using TR2.Domain;
using TR2.Protocol;
using Xunit;

namespace TR2.Application.Tests;

public sealed class DeviceTelemetrySnapshotRegistryTests
{
    private static readonly DeviceId Device = new(1234);
    private static readonly DateTimeOffset ReceivedAt =
        new(2026, 9, 9, 10, 0, 0, TimeSpan.Zero);

    [Fact]
    public void Unknown_device_starts_with_never_received_snapshots()
    {
        var registry = new DeviceTelemetrySnapshotRegistry();
        var policy = new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10));

        var snapshots = registry.Get(Device);

        Assert.Equal(SnapshotFreshness.NeverReceived, snapshots.SystemState.GetFreshness(policy, ReceivedAt));
        Assert.Equal(SnapshotFreshness.NeverReceived, snapshots.TimeState.GetFreshness(policy, ReceivedAt));
        Assert.Equal(SnapshotFreshness.NeverReceived, snapshots.VibrationState.GetFreshness(policy, ReceivedAt));
    }

    [Fact]
    public void Received_blocks_are_timestamped_independently()
    {
        var registry = new DeviceTelemetrySnapshotRegistry();
        var b1 = B1();
        var b2 = B2();
        var b3 = B3();

        registry.ReceiveSystemState(Device, b1, ReceivedAt);
        registry.ReceiveTimeState(Device, b2, ReceivedAt.AddSeconds(1));
        var snapshots = registry.ReceiveVibrationState(Device, b3, ReceivedAt.AddSeconds(2));

        Assert.Equal(b1, snapshots.SystemState.LastValue);
        Assert.Equal(ReceivedAt, snapshots.SystemState.ReceivedAt);
        Assert.Equal(b2, snapshots.TimeState.LastValue);
        Assert.Equal(ReceivedAt.AddSeconds(1), snapshots.TimeState.ReceivedAt);
        Assert.Equal(b3, snapshots.VibrationState.LastValue);
        Assert.Equal(ReceivedAt.AddSeconds(2), snapshots.VibrationState.ReceivedAt);
    }

    [Fact]
    public void Communication_loss_preserves_last_values_and_marks_them_unavailable()
    {
        var registry = new DeviceTelemetrySnapshotRegistry();
        var b1 = B1();
        var b2 = B2();
        var b3 = B3();
        registry.ReceiveSystemState(Device, b1, ReceivedAt);
        registry.ReceiveTimeState(Device, b2, ReceivedAt);
        registry.ReceiveVibrationState(Device, b3, ReceivedAt);
        var policy = new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10));

        var unavailable = registry.MarkUnavailable(Device);

        Assert.Equal(b1, unavailable.SystemState.LastValue);
        Assert.Equal(b2, unavailable.TimeState.LastValue);
        Assert.Equal(b3, unavailable.VibrationState.LastValue);
        Assert.Equal(SnapshotFreshness.Unavailable, unavailable.SystemState.GetFreshness(policy, ReceivedAt.AddSeconds(1)));
        Assert.Equal(SnapshotFreshness.Unavailable, unavailable.TimeState.GetFreshness(policy, ReceivedAt.AddSeconds(1)));
        Assert.Equal(SnapshotFreshness.Unavailable, unavailable.VibrationState.GetFreshness(policy, ReceivedAt.AddSeconds(1)));
    }

    [Fact]
    public void New_reception_after_unavailable_restores_availability_for_that_block_only()
    {
        var registry = new DeviceTelemetrySnapshotRegistry();
        registry.ReceiveSystemState(Device, B1(), ReceivedAt);
        registry.ReceiveTimeState(Device, B2(), ReceivedAt);
        registry.MarkUnavailable(Device);
        var policy = new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10));

        var updated = registry.ReceiveSystemState(Device, B1() with { SystemStatus = 3 }, ReceivedAt.AddSeconds(2));

        Assert.Equal(SnapshotFreshness.Fresh, updated.SystemState.GetFreshness(policy, ReceivedAt.AddSeconds(2)));
        Assert.Equal(SnapshotFreshness.Unavailable, updated.TimeState.GetFreshness(policy, ReceivedAt.AddSeconds(2)));
    }

    private static B1SystemState B1() =>
        new(1, 0, 0, 0, 100, 1, 250, 10, 20, 1, 30, 1, 42, 0, 0);

    private static B2TimeState B2() =>
        new(3, 1, 1000, 900, 100, 0, 0, 10, 1, 1);

    private static B3VibrationSupervision B3() =>
        new(3, 0, 0, 1, 1000, 0, 10, 1000, 100, 10, 20, 1, 2, 3, 4, 5, 6, 1, 0, 0, 0, 0, 0, 7, 8);
}
