using TR2.Application;
using TR2.Domain;
using TR2.Protocol;
using Xunit;

namespace TR2.Application.Tests;

public sealed class ReadProjectionConcurrencyTests
{
    [Fact]
    public async Task FleetAndTelemetryCanBeReadWhileRuntimeAuthoritiesAreUpdated()
    {
        var bus = new SerialBus("bus-1");
        var endpoint = new TR2Endpoint(bus, new ModbusAddress(1));
        var deviceId = new DeviceId(42);
        var device = new TR2Device(deviceId);
        var fleet = new FleetRegistry();
        fleet.RegisterEndpoint(endpoint);
        fleet.SetSession(TR2Session.CreateCompatible(endpoint, device));

        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var projection = new SupervisionReadProjection(
            fleet,
            telemetry,
            new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(30)));
        var startedAt = new DateTimeOffset(2026, 9, 9, 21, 0, 0, TimeSpan.Zero);

        var writer = Task.Run(() =>
        {
            for (var index = 0; index < 2000; index++)
            {
                fleet.SetSession(TR2Session.CreateCompatible(endpoint, device));
                telemetry.ReceiveVibrationState(
                    deviceId,
                    CreateVibration((uint)index),
                    startedAt.AddMilliseconds(index));
            }
        });

        var reader = Task.Run(() =>
        {
            for (var index = 0; index < 2000; index++)
            {
                var devices = projection.GetFleet(startedAt.AddSeconds(10));
                Assert.Single(devices);
                Assert.Equal(42u, devices[0].DeviceId);
            }
        });

        await Task.WhenAll(writer, reader);

        var final = Assert.Single(projection.GetFleet(startedAt.AddSeconds(10)));
        Assert.True(final.Telemetry!.VibrationState.HasValue);
    }

    private static B3VibrationSupervision CreateVibration(uint sequence) =>
        new(
            3, 1, 2, 4,
            1000, 50, sequence, 1000, 100,
            11, 22, 1, 2, 3, 4, 5, 6,
            2, 0, 0, 0, 0, 0, 7, 8);
}
