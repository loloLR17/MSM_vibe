using TR2.Application;
using TR2.Domain;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class PhysicalB5LifecycleConcurrencyTests
{
    [Fact]
    public async Task Concurrent_compatible_session_observations_queue_one_reconciliation_for_same_transaction()
    {
        var databasePath = Path.Combine(Path.GetTempPath(), $"tr2-supervision-j2-{Guid.NewGuid():N}.db");
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
                  "b5": {
                    "postSubmitPollIntervalMilliseconds": 500,
                    "postSubmitTimeoutMilliseconds": 30000,
                    "reconciliationIntervalMilliseconds": 2000
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
            var deviceId = new DeviceId(4004);
            composition.FleetRegistry.SetSession(
                TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));

            var coordinator = composition.CommandCoordinatorRegistry.GetOrAdd(
                deviceId,
                id => new CommandCoordinator(
                    id,
                    composition.CommandReservationStore,
                    composition.CommandJournal));
            var observedAt = DateTimeOffset.UtcNow;
            await coordinator.PrepareAsync("j2-ambiguous", observedAt);
            await coordinator.MarkAmbiguousAsync(observedAt);

            var runner = new PhysicalB5LifecycleWorkRunner(
                composition,
                new SupervisionOperationalFacade(composition));
            var start = new TaskCompletionSource(TaskCreationOptions.RunContinuationsAsynchronously);
            var observations = Enumerable.Range(0, 32)
                .Select(_ => Task.Run(async () =>
                {
                    await start.Task;
                    runner.ObserveCompatibleSession(endpoint, observedAt);
                }))
                .ToArray();

            start.SetResult();
            await Task.WhenAll(observations);

            var first = Assert.IsType<ScheduledBusWork>(
                composition.BusWorkScheduler.BeginNext(endpoint.Bus, observedAt.AddSeconds(5)));
            Assert.Equal(BusWorkKind.TransactionReconciliation, first.Kind);
            Assert.Null(composition.BusWorkScheduler.BeginNext(endpoint.Bus, observedAt.AddSeconds(5)));

            composition.BusWorkScheduler.Complete(endpoint.Bus, first.WorkId);
            Assert.Null(composition.BusWorkScheduler.BeginNext(endpoint.Bus, observedAt.AddSeconds(5)));
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
