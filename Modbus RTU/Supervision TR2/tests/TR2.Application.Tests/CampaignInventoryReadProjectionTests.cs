using TR2.Domain;
using TR2.Protocol;
using Xunit;

namespace TR2.Application.Tests;

public sealed class CampaignInventoryReadProjectionTests
{
    [Fact]
    public void B6_projection_preserves_selected_entry_and_storage_metadata()
    {
        var fleet = new FleetRegistry();
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var endpoint = new TR2Endpoint(new SerialBus("bus-1"), new ModbusAddress(4));
        var deviceId = new DeviceId(42);
        fleet.RegisterEndpoint(endpoint);
        fleet.SetSession(TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));

        var registers = new ushort[B6CampaignInventoryState.RegisterCount];
        registers[0] = 1;
        registers[1] = 7;
        registers[2] = 6;
        registers[3] = 2;
        registers[4] = 1;
        registers[5] = 0; registers[6] = 120;
        registers[7] = 0; registers[8] = 880;
        registers[9] = 1;
        registers[12] = 0; registers[13] = 33;
        registers[14] = 0; registers[15] = 9;
        registers[16] = 0; registers[17] = 1000;
        registers[18] = 0; registers[19] = 1600;
        registers[20] = 3;
        registers[21] = 0; registers[22] = 600;
        registers[23] = 0; registers[24] = 24;
        registers[25] = (ushort)(('C' << 8) | '1');
        registers[41] = (ushort)(('M' << 8) | '1');
        registers[57] = 1;

        var receivedAt = new DateTimeOffset(2026, 9, 10, 0, 10, 0, TimeSpan.Zero);
        telemetry.ReceiveCampaignInventoryState(deviceId, B6CampaignInventoryState.Parse(registers), receivedAt);

        var projection = new SupervisionReadProjection(
            fleet,
            telemetry,
            new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(30)));

        var campaign = Assert.Single(projection.GetFleet(receivedAt.AddSeconds(2))).Telemetry!.CampaignInventoryState;
        Assert.True(campaign.HasValue);
        Assert.True(campaign.IsAvailable);
        Assert.Equal(IhmSnapshotFreshness.Fresh, campaign.Freshness);
        Assert.Equal((ushort)7, campaign.Value!.TotalCampaignCount);
        Assert.Equal((ushort)6, campaign.Value.ValidCampaignCount);
        Assert.Equal((ushort)2, campaign.Value.SelectedCampaignIndex);
        Assert.Equal((ushort)1, campaign.Value.SelectedCampaignValid);
        Assert.Equal((uint)33, campaign.Value.CampaignId);
        Assert.Equal((uint)9, campaign.Value.MissionId);
        Assert.Equal((uint)600, campaign.Value.DurationSeconds);
        Assert.Equal("C1", campaign.Value.CampaignLabel);
        Assert.Equal("M1", campaign.Value.MissionLabel);
        Assert.Equal((ushort)1, campaign.Value.DataIntegrityStatus);
    }

    [Fact]
    public void Communication_loss_preserves_last_B6_value_but_marks_it_unavailable()
    {
        var fleet = new FleetRegistry();
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var endpoint = new TR2Endpoint(new SerialBus("bus-1"), new ModbusAddress(4));
        var deviceId = new DeviceId(42);
        fleet.RegisterEndpoint(endpoint);
        fleet.SetSession(TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));

        var registers = new ushort[B6CampaignInventoryState.RegisterCount];
        registers[12] = 0; registers[13] = 55;
        var receivedAt = new DateTimeOffset(2026, 9, 10, 0, 10, 0, TimeSpan.Zero);
        telemetry.ReceiveCampaignInventoryState(deviceId, B6CampaignInventoryState.Parse(registers), receivedAt);
        telemetry.MarkUnavailable(deviceId);

        var projection = new SupervisionReadProjection(
            fleet,
            telemetry,
            new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(30)));
        var campaign = Assert.Single(projection.GetFleet(receivedAt.AddSeconds(1))).Telemetry!.CampaignInventoryState;

        Assert.True(campaign.HasValue);
        Assert.False(campaign.IsAvailable);
        Assert.Equal(IhmSnapshotFreshness.Unavailable, campaign.Freshness);
        Assert.Equal((uint)55, campaign.Value!.CampaignId);
    }
}
