using TR2.Domain;
using TR2.Protocol;
using Xunit;

namespace TR2.Application.Tests;

public sealed class ConfigurationReadProjectionTests
{
    [Fact]
    public void B4_projection_preserves_prepared_and_active_values_and_pc_freshness()
    {
        var fleet = new FleetRegistry();
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var endpoint = new TR2Endpoint(new SerialBus("bus-1"), new ModbusAddress(4));
        var deviceId = new DeviceId(42);
        fleet.RegisterEndpoint(endpoint);
        fleet.SetSession(TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));

        var registers = new ushort[B4ConfigurationState.RegisterCount];
        registers[0] = 1;
        registers[2] = 0; registers[3] = 10;
        registers[4] = 0; registers[5] = 9;
        registers[16] = 1600;
        registers[41] = 120;
        registers[42] = 180;
        registers[100] = 800;
        registers[117] = 100;
        registers[118] = 150;
        var receivedAt = new DateTimeOffset(2026, 9, 10, 0, 0, 0, TimeSpan.Zero);
        telemetry.ReceiveConfigurationState(deviceId, B4ConfigurationState.Parse(registers), receivedAt);

        var projection = new SupervisionReadProjection(
            fleet,
            telemetry,
            new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(30)));

        var result = Assert.Single(projection.GetFleet(receivedAt.AddSeconds(2)));
        var configuration = result.Telemetry!.ConfigurationState;

        Assert.True(configuration.HasValue);
        Assert.True(configuration.IsAvailable);
        Assert.Equal(IhmSnapshotFreshness.Fresh, configuration.Freshness);
        Assert.Equal((uint)10, configuration.Value!.PreparedConfigId);
        Assert.Equal((uint)9, configuration.Value.ActiveConfigId);
        Assert.Equal((ushort)1600, configuration.Value.SamplingFrequencyHz);
        Assert.Equal((ushort)800, configuration.Value.ActiveSamplingFrequencyHz);
        Assert.Equal((ushort)120, configuration.Value.RmsWarnThresholdMg);
        Assert.Equal((ushort)100, configuration.Value.ActiveRmsWarnThresholdMg);
        Assert.Equal((ushort)180, configuration.Value.RmsAlarmThresholdMg);
        Assert.Equal((ushort)150, configuration.Value.ActiveRmsAlarmThresholdMg);
    }

    [Fact]
    public void Communication_loss_preserves_last_B4_value_but_marks_it_unavailable()
    {
        var fleet = new FleetRegistry();
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var endpoint = new TR2Endpoint(new SerialBus("bus-1"), new ModbusAddress(4));
        var deviceId = new DeviceId(42);
        fleet.RegisterEndpoint(endpoint);
        fleet.SetSession(TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));

        var registers = new ushort[B4ConfigurationState.RegisterCount];
        registers[4] = 0; registers[5] = 77;
        var receivedAt = new DateTimeOffset(2026, 9, 10, 0, 0, 0, TimeSpan.Zero);
        telemetry.ReceiveConfigurationState(deviceId, B4ConfigurationState.Parse(registers), receivedAt);
        telemetry.MarkUnavailable(deviceId);

        var projection = new SupervisionReadProjection(
            fleet,
            telemetry,
            new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(30)));
        var configuration = Assert.Single(projection.GetFleet(receivedAt.AddSeconds(1))).Telemetry!.ConfigurationState;

        Assert.True(configuration.HasValue);
        Assert.False(configuration.IsAvailable);
        Assert.Equal(IhmSnapshotFreshness.Unavailable, configuration.Freshness);
        Assert.Equal((uint)77, configuration.Value!.ActiveConfigId);
    }
}
