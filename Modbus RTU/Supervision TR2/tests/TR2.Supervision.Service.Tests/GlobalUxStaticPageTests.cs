using Microsoft.AspNetCore.Hosting.Server;
using Microsoft.AspNetCore.Hosting.Server.Features;
using Microsoft.Extensions.DependencyInjection;
using TR2.Application;
using TR2.Supervision.Web;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class GlobalUxStaticPageTests
{
    [Fact]
    public async Task Global_navigation_exposes_all_frozen_S6_destinations_and_real_read_scripts()
    {
        var projection = new SupervisionReadProjection(
            new FleetRegistry(),
            new DeviceTelemetrySnapshotRegistry(),
            new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10)));
        var host = new SupervisionWebHost(
            new SupervisionWebOptions(new Uri("http://127.0.0.1:0"), allowRemote: false),
            projection);

        await using var application = host.CreateApplication();
        await application.StartAsync();
        try
        {
            using var client = new HttpClient { BaseAddress = GetBaseAddress(application) };
            var pages = new[] { "/", "/vibrations.html", "/attention.html", "/campaigns.html", "/system.html" };
            foreach (var path in pages)
            {
                var html = await client.GetStringAsync(path);
                Assert.Contains("Vue générale", html, StringComparison.Ordinal);
                Assert.Contains("Vibrations", html, StringComparison.Ordinal);
                Assert.Contains("Attention & diagnostic", html, StringComparison.Ordinal);
                Assert.Contains("Campagnes", html, StringComparison.Ordinal);
                Assert.Contains("Système", html, StringComparison.Ordinal);
            }

            var vibrations = await client.GetStringAsync("/vibrations.html");
            Assert.Contains("RMS global", vibrations, StringComparison.Ordinal);
            Assert.Contains("Crête globale", vibrations, StringComparison.Ordinal);
            Assert.Contains("Sévérité B3", vibrations, StringComparison.Ordinal);
            Assert.DoesNotContain("mm/s", vibrations.Replace("Aucune vitesse mm/s", string.Empty), StringComparison.OrdinalIgnoreCase);

            var attention = await client.GetStringAsync("/attention.html");
            Assert.Contains("Communication PC", attention, StringComparison.OrdinalIgnoreCase);
            Assert.Contains("opérations B5", attention, StringComparison.OrdinalIgnoreCase);
            Assert.Contains("sources distinctes", attention, StringComparison.OrdinalIgnoreCase);

            var campaigns = await client.GetStringAsync("/campaigns.html");
            Assert.Contains("Inventaire B6", campaigns, StringComparison.Ordinal);
            Assert.DoesNotContain("selected_campaign_index", campaigns, StringComparison.Ordinal);

            var script = await client.GetStringAsync("/global-views.js");
            Assert.Contains("fetch('/api/v1/fleet'", script, StringComparison.Ordinal);
            Assert.Contains("/commands`,{headers", script, StringComparison.Ordinal);
            Assert.Contains("['Prepared','Submitted','Ambiguous']", script, StringComparison.Ordinal);
            Assert.Contains("durationSeconds", script, StringComparison.Ordinal);
            Assert.DoesNotContain("selectedCampaignIndex", script, StringComparison.Ordinal);
            Assert.DoesNotContain("WriteRegisters", script, StringComparison.OrdinalIgnoreCase);
        }
        finally
        {
            await application.StopAsync();
        }
    }

    private static Uri GetBaseAddress(Microsoft.AspNetCore.Builder.WebApplication application)
    {
        var server = application.Services.GetRequiredService<IServer>();
        var addresses = server.Features.Get<IServerAddressesFeature>()
            ?? throw new InvalidOperationException("Server addresses are unavailable.");
        return new Uri(Assert.Single(addresses.Addresses));
    }
}
