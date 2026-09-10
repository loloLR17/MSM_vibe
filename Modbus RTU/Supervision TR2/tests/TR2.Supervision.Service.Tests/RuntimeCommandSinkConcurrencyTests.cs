using TR2.Application;
using TR2.Domain;
using TR2.Supervision.Web;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class RuntimeCommandSinkConcurrencyTests
{
    [Fact]
    public async Task Concurrent_duplicate_request_identity_for_same_device_is_accepted_once()
    {
        var (runtime, databasePath) = await CreateRuntimeAsync();
        try
        {
            var endpoint = runtime.Composition.Configuration.Buses.Single().Endpoints[0];
            var deviceId = new DeviceId(5005);
            runtime.Composition.FleetRegistry.SetSession(TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));
            var sink = new RuntimeCommandSink(runtime);
            var observedAt = DateTimeOffset.UtcNow;
            var start = new TaskCompletionSource(TaskCreationOptions.RunContinuationsAsynchronously);

            async Task<IhmCommandQueueResult> QueueAsync()
            {
                await start.Task;
                return await sink.QueueAsync(
                    deviceId.Value,
                    "same-request",
                    new IhmB5CommandSubmission(IhmB5Command.StartAcquisition),
                    observedAt);
            }

            var attempts = Enumerable.Range(0, 16)
                .Select(_ => Task.Run(QueueAsync))
                .ToArray();
            start.SetResult();
            var results = await Task.WhenAll(attempts);

            Assert.Single(results, result => result.Status == IhmCommandQueueStatus.Accepted);
            Assert.Equal(15, results.Count(result => result.Status == IhmCommandQueueStatus.DuplicateRequestIdentity));
        }
        finally
        {
            DeleteDatabase(databasePath);
        }
    }

    [Fact]
    public async Task Different_devices_can_each_queue_a_command_through_the_same_web_sink()
    {
        var (runtime, databasePath) = await CreateRuntimeAsync();
        try
        {
            var endpoints = runtime.Composition.Configuration.Buses.Single().Endpoints;
            var firstId = new DeviceId(5005);
            var secondId = new DeviceId(5006);
            runtime.Composition.FleetRegistry.SetSession(TR2Session.CreateCompatible(endpoints[0], new TR2Device(firstId)));
            runtime.Composition.FleetRegistry.SetSession(TR2Session.CreateCompatible(endpoints[1], new TR2Device(secondId)));
            var sink = new RuntimeCommandSink(runtime);
            var observedAt = DateTimeOffset.UtcNow;
            var start = new TaskCompletionSource(TaskCreationOptions.RunContinuationsAsynchronously);

            async Task<IhmCommandQueueResult> QueueAsync(DeviceId deviceId, string requestIdentity)
            {
                await start.Task;
                return await sink.QueueAsync(
                    deviceId.Value,
                    requestIdentity,
                    new IhmB5CommandSubmission(IhmB5Command.StartAcquisition),
                    observedAt);
            }

            var first = Task.Run(() => QueueAsync(firstId, "device-5005"));
            var second = Task.Run(() => QueueAsync(secondId, "device-5006"));
            start.SetResult();
            var results = await Task.WhenAll(first, second);

            Assert.All(results, result => Assert.Equal(IhmCommandQueueStatus.Accepted, result.Status));
            Assert.Equal(2, runtime.Composition.CommandCoordinatorRegistry.Coordinators.Count);
        }
        finally
        {
            DeleteDatabase(databasePath);
        }
    }

    private static async Task<(PhysicalSupervisionRuntime Runtime, string DatabasePath)> CreateRuntimeAsync()
    {
        var databasePath = Path.Combine(Path.GetTempPath(), $"tr2-supervision-j3-{Guid.NewGuid():N}.db");
        var configuration = RuntimeConfigurationLoader.Parse(
            $$"""
            {
              "persistence": { "databasePath": "{{databasePath.Replace("\\", "\\\\")}}" },
              "polling": {
                "staticRetryMilliseconds": 20,
                "fastMilliseconds": 20,
                "mediumMilliseconds": 50,
                "slowMilliseconds": 100,
                "scanMilliseconds": 5
              },
              "buses": [
                { "id": "bus-1", "endpoints": [1, 2] }
              ]
            }
            """,
            Path.GetDirectoryName(databasePath)!);
        var runtime = PhysicalSupervisionRuntimeFactory.Create(configuration, supportedProtocolVersion: 1);
        await new SupervisionRuntimeStartup(runtime.Composition).StartAsync();
        return (runtime, databasePath);
    }

    private static void DeleteDatabase(string databasePath)
    {
        foreach (var suffix in new[] { string.Empty, "-wal", "-shm" })
        {
            var path = databasePath + suffix;
            if (File.Exists(path)) File.Delete(path);
        }
    }
}
