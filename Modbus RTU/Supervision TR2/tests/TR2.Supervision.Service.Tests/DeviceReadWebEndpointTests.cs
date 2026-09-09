using System.Net;
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

public sealed class DeviceReadWebEndpointTests
{
    private static readonly DateTimeOffset ObservedAt =
        new(2026, 9, 9, 22, 0, 0, TimeSpan.Zero);

    [Fact]
    public async Task Device_endpoint_returns_projected_B7_for_known_device()
    {
        var fleet = new FleetRegistry();
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var endpoint = new TR2Endpoint(new SerialBus("bus-1"), new ModbusAddress(4));
        var deviceId = new DeviceId(42);
        fleet.RegisterEndpoint(endpoint);
        fleet.SetSession(TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));
        telemetry.ReceiveDiagnosticState(
            deviceId,
            new B7DiagnosticState(1, 2, 0x0105, 17, 123456, 3, 9, 10, 654321, 2, 251, 3300),
            ObservedAt.AddSeconds(-1));

        var host = CreateHost(fleet, telemetry);
        await using var application = host.CreateApplication();
        await application.StartAsync();
        try
        {
            using var client = new HttpClient { BaseAddress = GetBaseAddress(application) };
            using var response = await client.GetAsync("/api/v1/devices/42");
            response.EnsureSuccessStatusCode();

            using var document = JsonDocument.Parse(await response.Content.ReadAsStringAsync());
            var root = document.RootElement;
            Assert.Equal(ObservedAt, root.GetProperty("observedAt").GetDateTimeOffset());
            var device = root.GetProperty("device");
            Assert.Equal(42u, device.GetProperty("deviceId").GetUInt32());
            var diagnostic = device.GetProperty("telemetry").GetProperty("diagnosticState");
            Assert.True(diagnostic.GetProperty("hasValue").GetBoolean());
            Assert.True(diagnostic.GetProperty("isAvailable").GetBoolean());
            Assert.Equal("Fresh", diagnostic.GetProperty("freshness").GetString());
            Assert.Equal(2, diagnostic.GetProperty("value").GetProperty("systemHealthStatus").GetInt32());
            Assert.Equal(17, diagnostic.GetProperty("value").GetProperty("lastFaultCode").GetInt32());
            Assert.Equal(3300, diagnostic.GetProperty("value").GetProperty("supplyVoltageMillivolts").GetInt32());
        }
        finally
        {
            await application.StopAsync();
        }
    }

    [Fact]
    public async Task Device_endpoint_returns_not_found_for_unknown_device_and_rejects_post()
    {
        var host = CreateHost(new FleetRegistry(), new DeviceTelemetrySnapshotRegistry());
        await using var application = host.CreateApplication();
        await application.StartAsync();
        try
        {
            using var client = new HttpClient { BaseAddress = GetBaseAddress(application) };

            using var getResponse = await client.GetAsync("/api/v1/devices/999");
            Assert.Equal(HttpStatusCode.NotFound, getResponse.StatusCode);

            using var postResponse = await client.PostAsync("/api/v1/devices/999", content: null);
            Assert.Equal(HttpStatusCode.MethodNotAllowed, postResponse.StatusCode);
        }
        finally
        {
            await application.StopAsync();
        }
    }

    private static SupervisionWebHost CreateHost(
        FleetRegistry fleet,
        DeviceTelemetrySnapshotRegistry telemetry) =>
        new(
            new SupervisionWebOptions(new Uri("http://127.0.0.1:0"), allowRemote: false),
            new SupervisionReadProjection(
                fleet,
                telemetry,
                new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(30))),
            new FixedTimeProvider(ObservedAt));

    private static Uri GetBaseAddress(Microsoft.AspNetCore.Builder.WebApplication application)
    {
        var server = application.Services.GetRequiredService<IServer>();
        var addresses = server.Features.Get<IServerAddressesFeature>()
            ?? throw new InvalidOperationException("Server addresses are unavailable.");
        return new Uri(Assert.Single(addresses.Addresses));
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
