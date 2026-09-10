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
    public async Task DeviceDetailPageServesReadViewsAndSupervisedActions()
    {
        var host = CreateHost();
        await using var application = host.CreateApplication();
        await application.StartAsync();
        try
        {
            using var client = new HttpClient { BaseAddress = GetBaseAddress(application) };
            var html = await client.GetStringAsync("/device.html?deviceId=42");
            var readScript = await client.GetStringAsync("/device.js");
            var commandScript = await client.GetStringAsync("/commands.js");
            var campaignScript = await client.GetStringAsync("/campaigns.js");

            Assert.Contains("Synthèse", html, StringComparison.Ordinal);
            Assert.Contains("Vibrations", html, StringComparison.Ordinal);
            Assert.Contains("État système", html, StringComparison.Ordinal);
            Assert.Contains("Configuration", html, StringComparison.Ordinal);
            Assert.Contains("Commandes", html, StringComparison.Ordinal);
            Assert.Contains("Campagnes", html, StringComparison.Ordinal);
            Assert.Contains("Diagnostic", html, StringComparison.Ordinal);
            Assert.Contains("Ambiguous = blocage opérateur", html, StringComparison.Ordinal);
            Assert.Contains("État transactionnel B5", html, StringComparison.Ordinal);
            Assert.Contains("Historique transactionnel", html, StringComparison.Ordinal);
            Assert.Contains("La sélection B6 n'est pas une commande B5", html, StringComparison.Ordinal);
            Assert.Contains("/api/v1/devices/${deviceId}", readScript, StringComparison.Ordinal);
            Assert.Contains("campaignInventoryState", readScript, StringComparison.Ordinal);
            Assert.Contains("selectedCampaignValid===1", readScript, StringComparison.Ordinal);
            Assert.DoesNotContain("selected_campaign_index", readScript, StringComparison.Ordinal);

            Assert.Contains("method:'POST'", commandScript, StringComparison.Ordinal);
            Assert.Contains("fetch(`/api/v1/devices/${commandDeviceId}/commands`", commandScript, StringComparison.Ordinal);
            Assert.Contains("['Prepared','Submitted','Ambiguous']", commandScript, StringComparison.Ordinal);
            Assert.Contains("transactionBlocked=true", commandScript, StringComparison.Ordinal);
            Assert.Contains("commandButtons.forEach(button=>button.disabled=busy||transactionBlocked)", commandScript, StringComparison.Ordinal);
            Assert.Contains("TerminalEvidenceObserved", commandScript, StringComparison.Ordinal);
            Assert.Contains("Aucun Retry / Ignore / Force", commandScript, StringComparison.Ordinal);
            Assert.Contains("setInterval(()=>void refreshTransactionState(),2000)", commandScript, StringComparison.Ordinal);
            Assert.Contains("AcknowledgeFault", commandScript, StringComparison.Ordinal);
            Assert.Contains("SoftwareReset", commandScript, StringComparison.Ordinal);
            Assert.Contains("Aucun retry automatique", commandScript, StringComparison.Ordinal);
            Assert.DoesNotContain("ResetStatistics", commandScript, StringComparison.Ordinal);
            Assert.DoesNotContain("0xA55A", commandScript, StringComparison.Ordinal);
            Assert.DoesNotContain("cmd_request_", commandScript, StringComparison.Ordinal);

            Assert.Contains("/api/v1/devices/${campaignDeviceId}/campaign-selection", campaignScript, StringComparison.Ordinal);
            Assert.Contains("campaignIndex", campaignScript, StringComparison.Ordinal);
            Assert.Contains("65535", campaignScript, StringComparison.Ordinal);
            Assert.Contains("Aucun retry automatique", campaignScript, StringComparison.Ordinal);
            Assert.DoesNotContain("6003", campaignScript, StringComparison.Ordinal);
            Assert.DoesNotContain("/commands", campaignScript, StringComparison.Ordinal);
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
