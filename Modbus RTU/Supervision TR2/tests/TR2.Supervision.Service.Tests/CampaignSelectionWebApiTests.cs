using System.Net;
using System.Text;
using Microsoft.AspNetCore.Hosting.Server;
using Microsoft.AspNetCore.Hosting.Server.Features;
using Microsoft.Extensions.DependencyInjection;
using TR2.Application;
using TR2.Supervision.Web;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class CampaignSelectionWebApiTests
{
    [Fact]
    public async Task Campaign_selection_is_forwarded_to_distinct_B6_sink_and_returns_202()
    {
        var sink = new StubCampaignSelectionSink(new IhmCampaignSelectionQueueResult(
            IhmCampaignSelectionQueueStatus.Accepted,
            WorkId: 321,
            DeviceId: 42,
            CampaignIndex: 7));
        var host = CreateHost(sink);
        await using var application = host.CreateApplication();
        await application.StartAsync();
        try
        {
            using var client = new HttpClient { BaseAddress = GetBaseAddress(application) };
            using var content = Json("{\"campaignIndex\":7}");
            var response = await client.PostAsync("/api/v1/devices/42/campaign-selection", content);
            var body = await response.Content.ReadAsStringAsync();

            Assert.Equal(HttpStatusCode.Accepted, response.StatusCode);
            Assert.Contains("\"campaignIndex\":7", body, StringComparison.Ordinal);
            Assert.Contains("\"workId\":321", body, StringComparison.Ordinal);
            Assert.Equal((uint)42, sink.DeviceId);
            Assert.Equal((ushort)7, sink.CampaignIndex);
        }
        finally
        {
            await application.StopAsync();
        }
    }

    [Fact]
    public async Task Maximum_ushort_index_is_not_rejected_by_web_business_rules()
    {
        var sink = new StubCampaignSelectionSink(new IhmCampaignSelectionQueueResult(
            IhmCampaignSelectionQueueStatus.Accepted,
            WorkId: 1,
            DeviceId: 42,
            CampaignIndex: ushort.MaxValue));
        var host = CreateHost(sink);
        await using var application = host.CreateApplication();
        await application.StartAsync();
        try
        {
            using var client = new HttpClient { BaseAddress = GetBaseAddress(application) };
            using var content = Json("{\"campaignIndex\":65535}");
            var response = await client.PostAsync("/api/v1/devices/42/campaign-selection", content);

            Assert.Equal(HttpStatusCode.Accepted, response.StatusCode);
            Assert.Equal(ushort.MaxValue, sink.CampaignIndex);
        }
        finally
        {
            await application.StopAsync();
        }
    }

    [Fact]
    public async Task Runtime_not_ready_is_reported_as_503()
    {
        var sink = new StubCampaignSelectionSink(new IhmCampaignSelectionQueueResult(
            IhmCampaignSelectionQueueStatus.NotReady,
            Detail: "The supervision runtime is not ready."));
        var host = CreateHost(sink);
        await using var application = host.CreateApplication();
        await application.StartAsync();
        try
        {
            using var client = new HttpClient { BaseAddress = GetBaseAddress(application) };
            using var content = Json("{\"campaignIndex\":2}");
            var response = await client.PostAsync("/api/v1/devices/42/campaign-selection", content);

            Assert.Equal(HttpStatusCode.ServiceUnavailable, response.StatusCode);
        }
        finally
        {
            await application.StopAsync();
        }
    }

    private static SupervisionWebHost CreateHost(ISupervisionCampaignSelectionSink sink)
    {
        var projection = new SupervisionReadProjection(
            new FleetRegistry(),
            new DeviceTelemetrySnapshotRegistry(),
            new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10)));
        return new SupervisionWebHost(
            new SupervisionWebOptions(new Uri("http://127.0.0.1:0"), allowRemote: false),
            projection,
            campaignSelectionSink: sink);
    }

    private static StringContent Json(string json) => new(json, Encoding.UTF8, "application/json");

    private static Uri GetBaseAddress(Microsoft.AspNetCore.Builder.WebApplication application)
    {
        var server = application.Services.GetRequiredService<IServer>();
        var addresses = server.Features.Get<IServerAddressesFeature>()
            ?? throw new InvalidOperationException("Server addresses are unavailable.");
        return new Uri(Assert.Single(addresses.Addresses));
    }

    private sealed class StubCampaignSelectionSink(IhmCampaignSelectionQueueResult result)
        : ISupervisionCampaignSelectionSink
    {
        public uint DeviceId { get; private set; }
        public ushort CampaignIndex { get; private set; }

        public ValueTask<IhmCampaignSelectionQueueResult> QueueCampaignSelectionAsync(
            uint deviceId,
            ushort campaignIndex,
            DateTimeOffset requestedAt,
            CancellationToken cancellationToken = default)
        {
            DeviceId = deviceId;
            CampaignIndex = campaignIndex;
            return ValueTask.FromResult(result);
        }
    }
}
