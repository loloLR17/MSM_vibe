using TR2.Domain;
using TR2.Protocol;
using Xunit;

namespace TR2.Application.Tests;

public sealed class DeviceDiagnosticReadProjectionTests
{
    private static readonly DateTimeOffset ReceivedAt =
        new(2026, 9, 9, 21, 45, 0, TimeSpan.Zero);

    [Fact]
    public void Device_projection_exposes_B7_without_fabricating_or_merging_B1_semantics()
    {
        var fleet = new FleetRegistry();
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var endpoint = new TR2Endpoint(new SerialBus("bus-1"), new ModbusAddress(4));
        var deviceId = new DeviceId(42);
        fleet.RegisterEndpoint(endpoint);
        fleet.SetSession(TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));
        telemetry.ReceiveDiagnosticState(deviceId, Diagnostic(), ReceivedAt);

        var projection = new SupervisionReadProjection(
            fleet,
            telemetry,
            new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(30)));

        var device = Assert.IsType<IhmDeviceReadModel>(
            projection.GetDevice(deviceId.Value, ReceivedAt.AddSeconds(2)));
        var diagnostic = Assert.IsType<IhmDeviceTelemetryReadModel>(device.Telemetry).DiagnosticState;

        Assert.True(diagnostic.HasValue);
        Assert.True(diagnostic.IsAvailable);
        Assert.Equal(IhmSnapshotFreshness.Fresh, diagnostic.Freshness);
        Assert.Equal(ReceivedAt, diagnostic.ReceivedAt);
        Assert.Equal((ushort)2, diagnostic.Value!.SystemHealthStatus);
        Assert.Equal((ushort)0x0105, diagnostic.Value.SystemFaultFlags);
        Assert.Equal((ushort)17, diagnostic.Value.LastFaultCode);
        Assert.Equal((uint)123456, diagnostic.Value.LastFaultTimestampSeconds);
        Assert.Equal((ushort)3, diagnostic.Value.SelftestStatus);
        Assert.Equal((short)251, diagnostic.Value.InternalTemperatureDeciCelsius);
        Assert.Equal((ushort)3300, diagnostic.Value.SupplyVoltageMillivolts);
        Assert.False(device.Telemetry!.SystemState.HasValue);
    }

    [Fact]
    public void Communication_loss_keeps_last_B7_value_and_marks_it_unavailable()
    {
        var fleet = new FleetRegistry();
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var endpoint = new TR2Endpoint(new SerialBus("bus-1"), new ModbusAddress(4));
        var deviceId = new DeviceId(42);
        fleet.RegisterEndpoint(endpoint);
        fleet.SetSession(TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));
        telemetry.ReceiveDiagnosticState(deviceId, Diagnostic(), ReceivedAt);
        telemetry.MarkUnavailable(deviceId);
        fleet.SetSession(fleet.GetSession(endpoint).MarkDisconnected());

        var projection = new SupervisionReadProjection(
            fleet,
            telemetry,
            new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(30)));

        var device = Assert.IsType<IhmDeviceReadModel>(
            projection.GetDevice(deviceId.Value, ReceivedAt.AddSeconds(1)));
        var diagnostic = Assert.IsType<IhmDeviceTelemetryReadModel>(device.Telemetry).DiagnosticState;

        Assert.Equal(IhmSessionState.Disconnected, device.SessionState);
        Assert.True(diagnostic.HasValue);
        Assert.False(diagnostic.IsAvailable);
        Assert.Equal(IhmSnapshotFreshness.Unavailable, diagnostic.Freshness);
        Assert.Equal((ushort)17, diagnostic.Value!.LastFaultCode);
    }

    [Fact]
    public void Unknown_device_id_returns_null()
    {
        var projection = new SupervisionReadProjection(
            new FleetRegistry(),
            new DeviceTelemetrySnapshotRegistry(),
            new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(30)));

        Assert.Null(projection.GetDevice(999, ReceivedAt));
    }

    private static B7DiagnosticState Diagnostic() =>
        new(
            DiagnosticStructureVersion: 1,
            SystemHealthStatus: 2,
            SystemFaultFlags: 0x0105,
            LastFaultCode: 17,
            LastFaultTimestampSeconds: 123456,
            SelftestStatus: 3,
            SelftestResultCode: 9,
            SelftestDetail: 10,
            UptimeSeconds: 654321,
            ResetCause: 2,
            InternalTemperatureDeciCelsius: 251,
            SupplyVoltageMillivolts: 3300);
}
