using TR2.Domain;
using TR2.Protocol;
using Xunit;

namespace TR2.Application.Tests;

public sealed class SupervisionReadProjectionTests
{
    private static readonly DateTimeOffset ReceivedAt =
        new(2026, 9, 9, 12, 0, 0, TimeSpan.Zero);

    private static readonly SnapshotFreshnessPolicy FreshnessPolicy =
        new(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10));

    [Fact]
    public void Fleet_projection_is_deterministic_and_uses_presentation_primitives()
    {
        var fleet = new FleetRegistry();
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var busB = new SerialBus("bus-b");
        var busA = new SerialBus("bus-a");
        var endpointB = new TR2Endpoint(busB, new ModbusAddress(1));
        var endpointA2 = new TR2Endpoint(busA, new ModbusAddress(2));
        var endpointA1 = new TR2Endpoint(busA, new ModbusAddress(1));

        fleet.RegisterEndpoint(endpointB);
        fleet.RegisterEndpoint(endpointA2);
        fleet.RegisterEndpoint(endpointA1);
        fleet.SetSession(TR2Session.CreateCompatible(endpointA2, new TR2Device(new DeviceId(2002))));
        fleet.SetSession(TR2Session.CreateIncompatible(endpointA1));

        var projection = new SupervisionReadProjection(fleet, telemetry, FreshnessPolicy);

        var result = projection.GetFleet(ReceivedAt);

        Assert.Collection(
            result,
            first =>
            {
                Assert.Equal("bus-a", first.BusId);
                Assert.Equal((byte)1, first.ModbusAddress);
                Assert.Null(first.DeviceId);
                Assert.Equal(IhmSessionState.Incompatible, first.SessionState);
                Assert.Null(first.Telemetry);
            },
            second =>
            {
                Assert.Equal("bus-a", second.BusId);
                Assert.Equal((byte)2, second.ModbusAddress);
                Assert.Equal((uint)2002, second.DeviceId);
                Assert.Equal(IhmSessionState.Compatible, second.SessionState);
                Assert.NotNull(second.Telemetry);
            },
            third =>
            {
                Assert.Equal("bus-b", third.BusId);
                Assert.Equal((byte)1, third.ModbusAddress);
                Assert.Null(third.DeviceId);
                Assert.Equal(IhmSessionState.Unidentified, third.SessionState);
                Assert.Null(third.Telemetry);
            });
    }

    [Fact]
    public void Known_device_without_telemetry_does_not_fabricate_values()
    {
        var (fleet, telemetry, endpoint, deviceId) = CompatibleDevice();
        var projection = new SupervisionReadProjection(fleet, telemetry, FreshnessPolicy);

        var result = Assert.Single(projection.GetFleet(ReceivedAt));

        Assert.Equal(deviceId.Value, result.DeviceId);
        var projected = Assert.IsType<IhmDeviceTelemetryReadModel>(result.Telemetry);
        Assert.False(projected.SystemState.HasValue);
        Assert.False(projected.SystemState.IsAvailable);
        Assert.Null(projected.SystemState.ReceivedAt);
        Assert.Equal(IhmSnapshotFreshness.NeverReceived, projected.SystemState.Freshness);
        Assert.Null(projected.SystemState.Value);
        Assert.False(projected.TimeState.HasValue);
        Assert.False(projected.VibrationState.HasValue);
    }

    [Fact]
    public void Telemetry_projection_preserves_received_values_and_pc_freshness()
    {
        var (fleet, telemetry, _, deviceId) = CompatibleDevice();
        telemetry.ReceiveSystemState(deviceId, B1(), ReceivedAt);
        telemetry.ReceiveTimeState(deviceId, B2(), ReceivedAt.AddSeconds(1));
        telemetry.ReceiveVibrationState(deviceId, B3(), ReceivedAt.AddSeconds(2));
        var projection = new SupervisionReadProjection(fleet, telemetry, FreshnessPolicy);

        var result = Assert.Single(projection.GetFleet(ReceivedAt.AddSeconds(4)));
        var projected = Assert.IsType<IhmDeviceTelemetryReadModel>(result.Telemetry);

        Assert.True(projected.SystemState.HasValue);
        Assert.True(projected.SystemState.IsAvailable);
        Assert.Equal(ReceivedAt, projected.SystemState.ReceivedAt);
        Assert.Equal(IhmSnapshotFreshness.Fresh, projected.SystemState.Freshness);
        Assert.Equal((ushort)1, projected.SystemState.Value!.SystemStatus);
        Assert.Equal((short)250, projected.SystemState.Value.InternalTemperatureDeciCelsius);

        Assert.Equal(ReceivedAt.AddSeconds(1), projected.TimeState.ReceivedAt);
        Assert.Equal((uint)1000, projected.TimeState.Value!.CurrentTimeSeconds);

        Assert.Equal(ReceivedAt.AddSeconds(2), projected.VibrationState.ReceivedAt);
        Assert.Equal((uint)10, projected.VibrationState.Value!.RmsGlobalMg);
        Assert.Equal((uint)20, projected.VibrationState.Value.PeakGlobalMg);
        Assert.Equal((uint)7, projected.VibrationState.Value.ExceedCount);
    }

    [Fact]
    public void Communication_loss_keeps_last_known_value_but_marks_projection_unavailable()
    {
        var (fleet, telemetry, endpoint, deviceId) = CompatibleDevice();
        telemetry.ReceiveVibrationState(deviceId, B3(), ReceivedAt);
        telemetry.MarkUnavailable(deviceId);
        fleet.SetSession(fleet.GetSession(endpoint).MarkDisconnected());
        var projection = new SupervisionReadProjection(fleet, telemetry, FreshnessPolicy);

        var result = Assert.Single(projection.GetFleet(ReceivedAt.AddSeconds(1)));
        var projected = Assert.IsType<IhmDeviceTelemetryReadModel>(result.Telemetry);

        Assert.Equal(IhmSessionState.Disconnected, result.SessionState);
        Assert.True(projected.VibrationState.HasValue);
        Assert.False(projected.VibrationState.IsAvailable);
        Assert.Equal(ReceivedAt, projected.VibrationState.ReceivedAt);
        Assert.Equal(IhmSnapshotFreshness.Unavailable, projected.VibrationState.Freshness);
        Assert.Equal((uint)10, projected.VibrationState.Value!.RmsGlobalMg);
    }

    private static (FleetRegistry Fleet, DeviceTelemetrySnapshotRegistry Telemetry, TR2Endpoint Endpoint, DeviceId DeviceId)
        CompatibleDevice()
    {
        var fleet = new FleetRegistry();
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var endpoint = new TR2Endpoint(new SerialBus("bus-1"), new ModbusAddress(3));
        var deviceId = new DeviceId(1234);
        fleet.RegisterEndpoint(endpoint);
        fleet.SetSession(TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));
        return (fleet, telemetry, endpoint, deviceId);
    }

    private static B1SystemState B1() =>
        new(1, 2, 3, 4, 100, 5, 250, 10, 20, 6, 30, 7, 42, 8, 9);

    private static B2TimeState B2() =>
        new(3, 1, 1000, 900, 100, 0, 0, 10, 1, 1);

    private static B3VibrationSupervision B3() =>
        new(3, 4, 5, 6, 1000, 0, 10, 1000, 100, 10, 20, 1, 2, 3, 4, 5, 6, 1, 0, 0, 0, 0, 0, 7, 8);
}
