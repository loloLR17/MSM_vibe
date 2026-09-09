using System.Net;
using System.Text;
using Microsoft.AspNetCore.Hosting.Server;
using Microsoft.AspNetCore.Hosting.Server.Features;
using Microsoft.Extensions.DependencyInjection;
using TR2.Application;
using TR2.Supervision.Web;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class CommandWebApiTests
{
    [Fact]
    public async Task Supported_command_is_forwarded_to_command_sink_and_returns_202()
    {
        var sink = new StubCommandSink(new IhmCommandQueueResult(
            IhmCommandQueueStatus.Accepted,
            WorkId: 123,
            DeviceId: 42,
            TransactionId: 7));
        var host = CreateHost(sink);
        await using var application = host.CreateApplication();
        await application.StartAsync();
        try
        {
            using var client = new HttpClient { BaseAddress = GetBaseAddress(application) };
            using var content = Json("{\"requestIdentity\":\"ui-1\",\"command\":\"StartAcquisition\"}");
            var response = await client.PostAsync("/api/v1/devices/42/commands", content);
            var body = await response.Content.ReadAsStringAsync();

            Assert.Equal(HttpStatusCode.Accepted, response.StatusCode);
            Assert.Contains("\"transactionId\":7", body, StringComparison.Ordinal);
            Assert.Equal((uint)42, sink.DeviceId);
            Assert.Equal("ui-1", sink.RequestIdentity);
            Assert.Equal(IhmB5Command.StartAcquisition, sink.Command);
        }
        finally
        {
            await application.StopAsync();
        }
    }

    [Fact]
    public async Task Missing_or_unsupported_command_is_rejected_before_command_sink()
    {
        var sink = new StubCommandSink(new IhmCommandQueueResult(IhmCommandQueueStatus.Accepted));
        var host = CreateHost(sink);
        await using var application = host.CreateApplication();
        await application.StartAsync();
        try
        {
            using var client = new HttpClient { BaseAddress = GetBaseAddress(application) };

            using var missing = Json("{\"requestIdentity\":\"ui-2\"}");
            var missingResponse = await client.PostAsync("/api/v1/devices/42/commands", missing);
            Assert.Equal(HttpStatusCode.BadRequest, missingResponse.StatusCode);

            using var unsupported = Json("{\"requestIdentity\":\"ui-3\",\"command\":\"ResetStatistics\"}");
            var unsupportedResponse = await client.PostAsync("/api/v1/devices/42/commands", unsupported);
            Assert.Equal(HttpStatusCode.BadRequest, unsupportedResponse.StatusCode);

            Assert.Equal(0, sink.CallCount);
        }
        finally
        {
            await application.StopAsync();
        }
    }

    [Fact]
    public async Task Duplicate_request_identity_is_reported_as_conflict()
    {
        var sink = new StubCommandSink(new IhmCommandQueueResult(
            IhmCommandQueueStatus.DuplicateRequestIdentity,
            Detail: "requestIdentity was already used for this device."));
        var host = CreateHost(sink);
        await using var application = host.CreateApplication();
        await application.StartAsync();
        try
        {
            using var client = new HttpClient { BaseAddress = GetBaseAddress(application) };
            using var content = Json("{\"requestIdentity\":\"same-request\",\"command\":\"StopAcquisition\"}");
            var response = await client.PostAsync("/api/v1/devices/42/commands", content);
            var body = await response.Content.ReadAsStringAsync();

            Assert.Equal(HttpStatusCode.Conflict, response.StatusCode);
            Assert.Contains("DuplicateRequestIdentity", body, StringComparison.Ordinal);
        }
        finally
        {
            await application.StopAsync();
        }
    }

    private static SupervisionWebHost CreateHost(ISupervisionCommandSink sink)
    {
        var projection = new SupervisionReadProjection(
            new FleetRegistry(),
            new DeviceTelemetrySnapshotRegistry(),
            new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10)));
        return new SupervisionWebHost(
            new SupervisionWebOptions(new Uri("http://127.0.0.1:0"), allowRemote: false),
            projection,
            commandSink: sink);
    }

    private static StringContent Json(string json) =>
        new(json, Encoding.UTF8, "application/json");

    private static Uri GetBaseAddress(Microsoft.AspNetCore.Builder.WebApplication application)
    {
        var server = application.Services.GetRequiredService<IServer>();
        var addresses = server.Features.Get<IServerAddressesFeature>()
            ?? throw new InvalidOperationException("Server addresses are unavailable.");
        return new Uri(Assert.Single(addresses.Addresses));
    }

    private sealed class StubCommandSink(IhmCommandQueueResult result) : ISupervisionCommandSink
    {
        public int CallCount { get; private set; }
        public uint DeviceId { get; private set; }
        public string? RequestIdentity { get; private set; }
        public IhmB5Command? Command { get; private set; }

        public ValueTask<IhmCommandQueueResult> QueueAsync(
            uint deviceId,
            string requestIdentity,
            IhmB5Command command,
            DateTimeOffset requestedAt,
            CancellationToken cancellationToken = default)
        {
            CallCount++;
            DeviceId = deviceId;
            RequestIdentity = requestIdentity;
            Command = command;
            return ValueTask.FromResult(result);
        }
    }
}
