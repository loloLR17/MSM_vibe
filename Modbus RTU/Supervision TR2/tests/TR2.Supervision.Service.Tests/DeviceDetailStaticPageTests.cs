using Microsoft.AspNetCore.Hosting.Server;
using Microsoft.AspNetCore.Hosting.Server.Features;
using Microsoft.Extensions.DependencyInjection;
using TR2.Application;
using TR2.Supervision.Web;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class DeviceDetailStaticPageTests
{
    [Fact]
    public async Task DeviceDetailPageIsServedAndRemainsReadOnly()
    {
        var host = CreateHost();
        await using var application = host.CreateApplication();
        await application.StartAsync();
        try
        {
            using var client = new HttpClient { BaseAddress = GetBaseAddress(application) };
            var html = await client.GetStringAsync("/device.html?deviceId=42");
            var script = await client.GetStringAsync("/device.js");

            Assert.Contains("Synthèse", html, StringComparison.Ordinal);
            Assert.Contains("Vibrations", html, StringComparison.Ordinal);
            Assert.Contains("État système", html, StringComparison.Ordinal);
            Assert.Contains("Diagnostic", html, StringComparison.Ordinal);
            Assert.Contains("Aucune commande B5", html, StringComparison.Ordinal);
            Assert.Contains("/api/v1/devices/${deviceId}", script, StringComparison.Ordinal);
            Assert.DoesNotContain("fetch('/api/v1/commands", script, StringComparison.Ordinal);
            Assert.DoesNotContain("method:'POST'", script, StringComparison.OrdinalIgnoreCase);
        }
        finally
        {
            await application.StopAsync();
        }
    }

    [Fact]
    public async Task FleetScriptLinksOnlyKnownDeviceIdsToDetailPage()
    {
        var host = CreateHost();
        await using var application = host.CreateApplication();
        await application.StartAsync();
        try
        {
            using var client = new HttpClient { BaseAddress = GetBaseAddress(application) };
            var script = await client.GetStringAsync("/app.js");

            Assert.Contains("d.deviceId==null", script, StringComparison.Ordinal);
            Assert.Contains("/device.html?deviceId=", script, StringComparison.Ordinal);
        }
        finally
        {
            await application.StopAsync();
        }
    }

    private static SupervisionWebHost CreateHost()
    {
        var projection = new SupervisionReadProjection(
            new FleetRegistry(),
            new DeviceTelemetrySnapshotRegistry(),
            new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10)));
        return new SupervisionWebHost(
            new SupervisionWebOptions(new Uri("http://127.0.0.1:0"), allowRemote: false),
            projection);
    }

    private static Uri GetBaseAddress(Microsoft.AspNetCore.Builder.WebApplication application)
    {
        var server = application.Services.GetRequiredService<IServer>();
        var addresses = server.Features.Get<IServerAddressesFeature>()
            ?? throw new InvalidOperationException("Server addresses are unavailable.");
        return new Uri(Assert.Single(addresses.Addresses));
    }
}
