using System.Text.Json;
using Microsoft.AspNetCore.Hosting.Server;
using Microsoft.AspNetCore.Hosting.Server.Features;
using Microsoft.Extensions.DependencyInjection;
using TR2.Application;
using TR2.Domain;
using TR2.Protocol;
using TR2.Supervision.Web;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class SupervisionWebHostTests
{
    private static readonly DateTimeOffset ObservedAt =
        new(2026, 9, 9, 20, 0, 0, TimeSpan.Zero);

    [Fact]
    public void OptionsRejectRemoteListeningWithoutExplicitOptIn()
    {
        var exception = Assert.Throws<ArgumentException>(() =>
            new SupervisionWebOptions(new Uri("http://0.0.0.0:5080"), allowRemote: false));

        Assert.Contains("allowRemote=true", exception.Message, StringComparison.Ordinal);
    }

    [Fact]
    public async Task FleetEndpointSerializesReadProjectionAndObservationTime()
    {
        var bus = new SerialBus("bus-1");
        var endpoint = new TR2Endpoint(bus, new ModbusAddress(7));
        var deviceId = new DeviceId(42);
        var fleet = new FleetRegistry();
        fleet.RegisterEndpoint(endpoint);
        fleet.SetSession(TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));

        var telemetry = new DeviceTelemetrySnapshotRegistry();
        telemetry.ReceiveVibrationState(
            deviceId,
            new B3VibrationSupervision(
                3, 1, 2, 4,
                1000, 50, 10, 1000, 100,
                11, 22, 1, 2, 3, 4, 5, 6,
                2, 0, 0, 0, 0, 0, 7, 8),
            ObservedAt.AddSeconds(-1));

        var projection = new SupervisionReadProjection(
            fleet,
            telemetry,
            new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10)));
        var host = new SupervisionWebHost(
            new SupervisionWebOptions(new Uri("http://127.0.0.1:0"), allowRemote: false),
            projection,
            new FixedTimeProvider(ObservedAt));

        await using var application = host.CreateApplication();
        await application.StartAsync();
        try
        {
            var server = application.Services.GetRequiredService<IServer>();
            var addresses = server.Features.Get<IServerAddressesFeature>()
                ?? throw new InvalidOperationException("Server addresses are unavailable.");
            var baseAddress = new Uri(Assert.Single(addresses.Addresses));

            using var client = new HttpClient { BaseAddress = baseAddress };
            using var response = await client.GetAsync("/api/v1/fleet");
            response.EnsureSuccessStatusCode();

            using var document = JsonDocument.Parse(await response.Content.ReadAsStringAsync());
            var root = document.RootElement;
            Assert.Equal(ObservedAt, root.GetProperty("observedAt").GetDateTimeOffset());

            var devices = root.GetProperty("devices");
            Assert.Equal(1, devices.GetArrayLength());
            var device = devices[0];
            Assert.Equal("bus-1", device.GetProperty("busId").GetString());
            Assert.Equal(7, device.GetProperty("modbusAddress").GetInt32());
            Assert.Equal(42u, device.GetProperty("deviceId").GetUInt32());
            Assert.Equal("Compatible", device.GetProperty("sessionState").GetString());

            var vibration = device
                .GetProperty("telemetry")
                .GetProperty("vibrationState");
            Assert.True(vibration.GetProperty("hasValue").GetBoolean());
            Assert.True(vibration.GetProperty("isAvailable").GetBoolean());
            Assert.Equal("Fresh", vibration.GetProperty("freshness").GetString());
            Assert.Equal(11u, vibration.GetProperty("value").GetProperty("rmsGlobalMg").GetUInt32());
        }
        finally
        {
            await application.StopAsync();
        }
    }

    [Fact]
    public async Task FleetEndpointDoesNotAcceptPost()
    {
        var projection = new SupervisionReadProjection(
            new FleetRegistry(),
            new DeviceTelemetrySnapshotRegistry(),
            new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10)));
        var host = new SupervisionWebHost(
            new SupervisionWebOptions(new Uri("http://127.0.0.1:0"), allowRemote: false),
            projection,
            new FixedTimeProvider(ObservedAt));

        await using var application = host.CreateApplication();
        await application.StartAsync();
        try
        {
            var server = application.Services.GetRequiredService<IServer>();
            var addresses = server.Features.Get<IServerAddressesFeature>()
                ?? throw new InvalidOperationException("Server addresses are unavailable.");
            var baseAddress = new Uri(Assert.Single(addresses.Addresses));

            using var client = new HttpClient { BaseAddress = baseAddress };
            using var response = await client.PostAsync("/api/v1/fleet", content: null);

            Assert.Equal(System.Net.HttpStatusCode.MethodNotAllowed, response.StatusCode);
        }
        finally
        {
            await application.StopAsync();
        }
    }

    private sealed class FixedTimeProvider : TimeProvider
    {
        private readonly DateTimeOffset _utcNow;

        public FixedTimeProvider(DateTimeOffset utcNow)
        {
            _utcNow = utcNow;
        }

        public override DateTimeOffset GetUtcNow() => _utcNow;
    }
}
