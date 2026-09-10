using System.Net;
using System.Text.Json;
using Microsoft.AspNetCore.Hosting.Server;
using Microsoft.AspNetCore.Hosting.Server.Features;
using Microsoft.Extensions.DependencyInjection;
using TR2.Application;
using TR2.Supervision.Web;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class CommandHistoryWebApiTests
{
    [Fact]
    public async Task Command_history_exposes_presentation_states_without_claiming_success()
    {
        var source = new StubSource([
            new IhmB5TransactionReadModel(42, 7, "web-a", IhmB5TransactionState.Ambiguous, new DateTimeOffset(2026, 9, 10, 9, 0, 0, TimeSpan.Zero)),
            new IhmB5TransactionReadModel(42, 6, "web-b", IhmB5TransactionState.TerminalEvidenceObserved, new DateTimeOffset(2026, 9, 10, 8, 0, 0, TimeSpan.Zero))
        ]);
        var host = CreateHost(source);
        await using var application = host.CreateApplication();
        await application.StartAsync();
        try
        {
            using var client = new HttpClient { BaseAddress = GetBaseAddress(application) };
            var response = await client.GetAsync("/api/v1/devices/42/commands");
            Assert.Equal(HttpStatusCode.OK, response.StatusCode);
            var body = await response.Content.ReadAsStringAsync();
            Assert.Contains("\"state\":\"Ambiguous\"", body, StringComparison.Ordinal);
            Assert.Contains("\"state\":\"TerminalEvidenceObserved\"", body, StringComparison.Ordinal);
            Assert.Contains("\"requestIdentity\":\"web-a\"", body, StringComparison.Ordinal);
            Assert.DoesNotContain("Succeeded", body, StringComparison.OrdinalIgnoreCase);
            Assert.Equal((uint)42, source.DeviceId);
        }
        finally
        {
            await application.StopAsync();
        }
    }

    [Fact]
    public async Task Command_history_is_not_available_without_authoritative_source()
    {
        var host = CreateHost(null);
        await using var application = host.CreateApplication();
        await application.StartAsync();
        try
        {
            using var client = new HttpClient { BaseAddress = GetBaseAddress(application) };
            Assert.Equal(HttpStatusCode.NotFound, (await client.GetAsync("/api/v1/devices/42/commands")).StatusCode);
        }
        finally
        {
            await application.StopAsync();
        }
    }

    private static SupervisionWebHost CreateHost(ISupervisionCommandHistoryReadSource? source)
    {
        var projection = new SupervisionReadProjection(
            new FleetRegistry(),
            new DeviceTelemetrySnapshotRegistry(),
            new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10)));
        return new SupervisionWebHost(
            new SupervisionWebOptions(new Uri("http://127.0.0.1:0"), allowRemote: false),
            projection,
            commandHistoryReadSource: source);
    }

    private static Uri GetBaseAddress(Microsoft.AspNetCore.Builder.WebApplication application)
    {
        var server = application.Services.GetRequiredService<IServer>();
        var addresses = server.Features.Get<IServerAddressesFeature>()
            ?? throw new InvalidOperationException("Server addresses are unavailable.");
        return new Uri(Assert.Single(addresses.Addresses));
    }

    private sealed class StubSource(IReadOnlyList<IhmB5TransactionReadModel> transactions) : ISupervisionCommandHistoryReadSource
    {
        public uint? DeviceId { get; private set; }

        public ValueTask<IReadOnlyList<IhmB5TransactionReadModel>> ReadAsync(uint deviceId, CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            DeviceId = deviceId;
            return ValueTask.FromResult(transactions);
        }
    }
}
