using TR2.Application;
using TR2.Domain;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class SupervisionConcurrentCommandTests
{
    [Fact]
    public async Task Concurrent_requests_for_same_device_create_one_coordinator_and_one_nonterminal_command()
    {
        var databasePath = Path.Combine(Path.GetTempPath(), $"tr2-supervision-j1-{Guid.NewGuid():N}.db");
        try
        {
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
                    { "id": "bus-1", "endpoints": [1] }
                  ]
                }
                """,
                Path.GetDirectoryName(databasePath)!);
            var composition = SupervisionRuntimeCompositionRoot.Compose(configuration);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            var deviceId = new DeviceId(3003);
            composition.FleetRegistry.SetSession(
                TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));
            var facade = new SupervisionOperationalFacade(composition);
            var dueAt = DateTimeOffset.UtcNow;
            var start = new TaskCompletionSource(TaskCreationOptions.RunContinuationsAsynchronously);

            async Task<QueuedB5Command?> TryQueueAsync(int index)
            {
                await start.Task;
                try
                {
                    return await facade.QueueCommandAsync(
                        endpoint,
                        $"concurrent-{index}",
                        new B5CommandIntent(3, 0, 0, 0, 0),
                        dueAt);
                }
                catch (InvalidOperationException)
                {
                    return null;
                }
            }

            var attempts = Enumerable.Range(0, 24)
                .Select(index => Task.Run(() => TryQueueAsync(index)))
                .ToArray();
            start.SetResult();
            var results = await Task.WhenAll(attempts);

            var accepted = Assert.Single(results.Where(result => result is not null));
            Assert.Single(composition.CommandCoordinatorRegistry.Coordinators);
            var coordinator = composition.CommandCoordinatorRegistry.Get(deviceId);
            Assert.NotNull(coordinator.ActiveTransaction);
            Assert.Equal(CommandTransactionState.Prepared, coordinator.ActiveTransaction!.State);

            var work = Assert.IsType<ScheduledBusWork>(
                composition.BusWorkScheduler.BeginNext(endpoint.Bus, dueAt.AddSeconds(1)));
            Assert.Equal(BusWorkKind.CommandTransaction, work.Kind);
            Assert.Equal(accepted!.Request, facade.GetCommandRequest(work));
            Assert.Null(composition.BusWorkScheduler.BeginNext(endpoint.Bus, dueAt.AddSeconds(1)));
            composition.BusWorkScheduler.Complete(endpoint.Bus, work.WorkId);
            Assert.Null(composition.BusWorkScheduler.BeginNext(endpoint.Bus, dueAt.AddSeconds(1)));
        }
        finally
        {
            foreach (var suffix in new[] { string.Empty, "-wal", "-shm" })
            {
                var path = databasePath + suffix;
                if (File.Exists(path)) File.Delete(path);
            }
        }
    }
}
