using System.Text.Json;
using Microsoft.AspNetCore.Hosting.Server;
using Microsoft.AspNetCore.Hosting.Server.Features;
using Microsoft.Extensions.DependencyInjection;
using TR2.Application;
using TR2.Supervision.Web;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class SystemWebViewTests
{
    [Fact]
    public async Task SystemApiProjectsRuntimeAndCommunicationFailuresReadOnly()
    {
        var source = new FakeSystemReadSource();
        var host = CreateHost(source);
        await using var application = host.CreateApplication();
        await application.StartAsync();
        try
        {
            using var client = new HttpClient { BaseAddress = GetBaseAddress(application) };
            using var document = JsonDocument.Parse(await client.GetStringAsync("/api/v1/system"));
            var system = document.RootElement.GetProperty("system");

            Assert.Equal("Running", system.GetProperty("hostState").GetString());
            Assert.True(system.GetProperty("isReady").GetBoolean());
            Assert.Equal("bus-1", system.GetProperty("connectedBusIds")[0].GetString());
            Assert.Equal("Compatible", system.GetProperty("endpoints")[0].GetProperty("sessionState").GetString());
            Assert.Equal("Timeout", system.GetProperty("recentCommunicationFailures")[0].GetProperty("category").GetString());
        }
        finally
        {
            await application.StopAsync();
        }
    }

    [Fact]
    public async Task SystemPageIsServedAndContainsNoWriteAction()
    {
        var host = CreateHost(new FakeSystemReadSource());
        await using var application = host.CreateApplication();
        await application.StartAsync();
        try
        {
            using var client = new HttpClient { BaseAddress = GetBaseAddress(application) };
            var html = await client.GetStringAsync("/system.html");
            var script = await client.GetStringAsync("/system.js");

            Assert.Contains("Système & communications", html, StringComparison.Ordinal);
            Assert.Contains("Aucune action runtime", html, StringComparison.Ordinal);
            Assert.Contains("/api/v1/system", script, StringComparison.Ordinal);
            Assert.DoesNotContain("method:'POST'", script, StringComparison.OrdinalIgnoreCase);
            Assert.DoesNotContain("innerHTML", script, StringComparison.OrdinalIgnoreCase);
        }
        finally
        {
            await application.StopAsync();
        }
    }

    private static SupervisionWebHost CreateHost(ISupervisionSystemReadSource source)
    {
        var projection = new SupervisionReadProjection(
            new FleetRegistry(),
            new DeviceTelemetrySnapshotRegistry(),
            new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10)));
        return new SupervisionWebHost(
            new SupervisionWebOptions(new Uri("http://127.0.0.1:0"), allowRemote: false),
            projection,
            systemReadSource: source);
    }

    private static Uri GetBaseAddress(Microsoft.AspNetCore.Builder.WebApplication application)
    {
        var server = application.Services.GetRequiredService<IServer>();
        var addresses = server.Features.Get<IServerAddressesFeature>()
            ?? throw new InvalidOperationException("Server addresses are unavailable.");
        return new Uri(Assert.Single(addresses.Addresses));
    }

    private sealed class FakeSystemReadSource : ISupervisionSystemReadSource
    {
        public IhmSystemReadModel Read() => new(
            "Running",
            true,
            ["bus-1"],
            [new IhmRuntimeEndpointReadModel("bus-1", 4, "Compatible", 42)],
            [new IhmCommunicationFailureReadModel(
                12,
                "bus-1",
                4,
                42,
                "Polling",
                "Timeout",
                new DateTimeOffset(2026, 9, 10, 0, 20, 0, TimeSpan.Zero),
                "TimeoutException",
                "timeout")]);
    }
}
