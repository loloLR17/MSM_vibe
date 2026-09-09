using TR2.Domain;
using TR2.Protocol;
using Xunit;

namespace TR2.Application.Tests;

public sealed class PollingTelemetryPublisherTests
{
    private static readonly DateTimeOffset ReceivedAt =
        new(2026, 9, 9, 12, 30, 0, TimeSpan.Zero);

    [Fact]
    public void Fast_polling_publishes_B1_and_B3_without_touching_B2()
    {
        var endpoint = Endpoint();
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var publisher = Publisher(endpoint, telemetry);
        var b1 = B1();
        var b3 = B3();
        var readSet = new PollingReadSet(
            PollingGroup.Fast,
            null,
            b1,
            null,
            b3,
            null,
            null,
            null,
            null);

        var published = publisher.Publish(endpoint, readSet, ReceivedAt)!;

        Assert.Equal(b1, published.SystemState.LastValue);
        Assert.Equal(ReceivedAt, published.SystemState.ReceivedAt);
        Assert.False(published.TimeState.HasValue);
        Assert.Equal(b3, published.VibrationState.LastValue);
        Assert.Equal(ReceivedAt, published.VibrationState.ReceivedAt);
    }

    [Fact]
    public void Medium_polling_publishes_B2_only_to_telemetry_registry()
    {
        var endpoint = Endpoint();
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var publisher = Publisher(endpoint, telemetry);
        var b2 = B2();
        var readSet = new PollingReadSet(
            PollingGroup.Medium,
            null,
            null,
            b2,
            null,
            null,
            null,
            null,
            null);

        var published = publisher.Publish(endpoint, readSet, ReceivedAt)!;

        Assert.False(published.SystemState.HasValue);
        Assert.Equal(b2, published.TimeState.LastValue);
        Assert.Equal(ReceivedAt, published.TimeState.ReceivedAt);
        Assert.False(published.VibrationState.HasValue);
    }

    [Fact]
    public void Slow_or_static_data_does_not_create_fake_telemetry()
    {
        var endpoint = Endpoint();
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var publisher = Publisher(endpoint, telemetry);
        var readSet = new PollingReadSet(
            PollingGroup.Slow,
            null,
            null,
            null,
            null,
            null,
            null,
            null,
            null);

        var published = publisher.Publish(endpoint, readSet, ReceivedAt);
        var snapshots = telemetry.Get(new DeviceId(1001));

        Assert.Null(published);
        Assert.False(snapshots.SystemState.HasValue);
        Assert.False(snapshots.TimeState.HasValue);
        Assert.False(snapshots.VibrationState.HasValue);
    }

    [Fact]
    public void Telemetry_publication_requires_current_compatible_device_identity()
    {
        var endpoint = Endpoint();
        var fleet = new FleetRegistry();
        fleet.RegisterEndpoint(endpoint);
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var publisher = new PollingTelemetryPublisher(fleet, telemetry);
        var readSet = new PollingReadSet(
            PollingGroup.Fast,
            null,
            B1(),
            null,
            B3(),
            null,
            null,
            null,
            null);

        Assert.Throws<InvalidOperationException>(() =>
            publisher.Publish(endpoint, readSet, ReceivedAt));
    }

    [Fact]
    public void Explicit_unavailable_mark_preserves_last_values()
    {
        var endpoint = Endpoint();
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var publisher = Publisher(endpoint, telemetry);
        var readSet = new PollingReadSet(
            PollingGroup.Fast,
            null,
            B1(),
            null,
            B3(),
            null,
            null,
            null,
            null);
        publisher.Publish(endpoint, readSet, ReceivedAt);

        var unavailable = publisher.MarkUnavailable(endpoint)!;

        Assert.True(unavailable.SystemState.HasValue);
        Assert.False(unavailable.SystemState.IsAvailable);
        Assert.True(unavailable.VibrationState.HasValue);
        Assert.False(unavailable.VibrationState.IsAvailable);
        Assert.False(unavailable.TimeState.IsAvailable);
        Assert.Equal(readSet.B1, unavailable.SystemState.LastValue);
        Assert.Equal(readSet.B3, unavailable.VibrationState.LastValue);
    }

    [Fact]
    public void Unidentified_endpoint_has_no_device_snapshot_to_mark_unavailable()
    {
        var endpoint = Endpoint();
        var fleet = new FleetRegistry();
        fleet.RegisterEndpoint(endpoint);
        var publisher = new PollingTelemetryPublisher(
            fleet,
            new DeviceTelemetrySnapshotRegistry());

        Assert.Null(publisher.MarkUnavailable(endpoint));
    }

    private static PollingTelemetryPublisher Publisher(
        TR2Endpoint endpoint,
        DeviceTelemetrySnapshotRegistry telemetry)
    {
        var fleet = new FleetRegistry();
        fleet.RegisterEndpoint(endpoint);
        fleet.SetSession(TR2Session.CreateCompatible(
            endpoint,
            new TR2Device(new DeviceId(1001))));
        return new PollingTelemetryPublisher(fleet, telemetry);
    }

    private static TR2Endpoint Endpoint() =>
        new(new SerialBus("RS485-A"), new ModbusAddress(10));

    private static B1SystemState B1() =>
        new(1, 0, 0, 0, 100, 1, 250, 10, 20, 1, 30, 1, 42, 0, 0);

    private static B2TimeState B2() =>
        new(3, 1, 1000, 900, 100, 0, 0, 10, 1, 1);

    private static B3VibrationSupervision B3() =>
        new(3, 0, 0, 1, 1000, 0, 10, 1000, 100, 10, 20, 1, 2, 3, 4, 5, 6, 1, 0, 0, 0, 0, 0, 7, 8);
}
