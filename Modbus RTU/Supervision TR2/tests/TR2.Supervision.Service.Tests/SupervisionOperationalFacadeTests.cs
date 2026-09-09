using TR2.Application;
using TR2.Domain;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class SupervisionOperationalFacadeTests
{
    [Fact]
    public async Task FacadeRequiresReadinessThenPreparesDurableB5AndRefreshContexts()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var composition = Compose(databasePath);
            var facade = new SupervisionOperationalFacade(composition);
            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            var intent = new B5CommandIntent(3, 10, 20, 30, 0x55AA);

            await Assert.ThrowsAsync<InvalidOperationException>(async () =>
                await facade.QueueCommandAsync(endpoint, "operator-request-1", intent, DateTimeOffset.UtcNow));

            await new SupervisionRuntimeStartup(composition).StartAsync();
            var deviceId = new DeviceId(1001);
            composition.FleetRegistry.SetSession(
                TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));

            var queued = await facade.QueueCommandAsync(
                endpoint,
                "operator-request-1",
                intent,
                DateTimeOffset.UtcNow);

            Assert.Equal(BusWorkKind.CommandTransaction, queued.Work.Kind);
            Assert.Equal(deviceId, queued.DeviceId);
            Assert.NotEqual((ushort)0, queued.Request.TransactionId);
            Assert.Equal(queued.Request, facade.GetCommandRequest(queued.Work));

            var coordinator = composition.CommandCoordinatorRegistry.Get(deviceId);
            Assert.NotNull(coordinator.ActiveTransaction);
            Assert.Equal(CommandTransactionState.Prepared, coordinator.ActiveTransaction!.State);
            Assert.Equal(queued.Request.TransactionId, coordinator.ActiveTransaction.TransactionId.Value);

            var refreshes = facade.QueuePostReconnectRefresh(endpoint, DateTimeOffset.UtcNow);
            Assert.Equal(TR2PollingPlan.PostReconnectRefreshBlocks.Count, refreshes.Count);
            Assert.All(refreshes, refresh =>
            {
                Assert.Equal(BusWorkKind.ExplicitRefresh, refresh.Work.Kind);
                Assert.Equal(refresh, facade.GetRefresh(refresh.Work));
            });
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task UnifiedLoopDispatchesPriorityWorkBeforeDuePolling()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var composition = Compose(databasePath);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            composition.FleetRegistry.SetSession(
                TR2Session.CreateCompatible(endpoint, new TR2Device(new DeviceId(2002))));

            var facade = new SupervisionOperationalFacade(composition);
            var now = DateTimeOffset.UtcNow;
            composition.BusWorkScheduler.QueuePolling(endpoint, PollingGroup.Fast, now);
            var command = await facade.QueueCommandAsync(
                endpoint,
                "priority-request",
                new B5CommandIntent(5, 1, 2, 3, 4),
                now);

            var sequence = new List<BusWorkKind>();
            var pollingRunner = new RecordingPollingRunner(composition, sequence);
            var priorityRunner = new RecordingPriorityRunner(composition, facade, sequence);
            var loop = new SupervisionPollingLoop(composition, pollingRunner, priorityRunner);
            using var cancellation = new CancellationTokenSource();

            var runTask = loop.RunAsync(cancellation.Token);
            await WaitForAsync(() => sequence.Count >= 2);
            cancellation.Cancel();

            await Assert.ThrowsAnyAsync<OperationCanceledException>(async () => await runTask);

            Assert.Equal(BusWorkKind.CommandTransaction, sequence[0]);
            Assert.Equal(BusWorkKind.Polling, sequence[1]);
            Assert.Equal(command.Request, priorityRunner.Requests.Single());
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    private static SupervisionRuntimeComposition Compose(string databasePath)
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

        return SupervisionRuntimeCompositionRoot.Compose(configuration);
    }

    private static async Task WaitForAsync(Func<bool> predicate)
    {
        using var timeout = new CancellationTokenSource(TimeSpan.FromSeconds(5));
        while (!predicate())
        {
            await Task.Delay(10, timeout.Token);
        }
    }

    private static string NewDatabasePath() =>
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s3f-{Guid.NewGuid():N}.db");

    private static void DeleteDatabaseFiles(string databasePath)
    {
        foreach (var suffix in new[] { string.Empty, "-wal", "-shm" })
        {
            var path = databasePath + suffix;
            if (File.Exists(path))
            {
                File.Delete(path);
            }
        }
    }

    private sealed class RecordingPollingRunner : IPollingWorkRunner
    {
        private readonly SupervisionRuntimeComposition _composition;
        private readonly List<BusWorkKind> _sequence;

        public RecordingPollingRunner(
            SupervisionRuntimeComposition composition,
            List<BusWorkKind> sequence)
        {
            _composition = composition;
            _sequence = sequence;
        }

        public ValueTask<PollingWorkExecutionResult> ExecuteAsync(
            ScheduledBusWork work,
            DateTimeOffset observedAt,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            _sequence.Add(work.Kind);
            _composition.BusWorkScheduler.Complete(work.Endpoint.Bus, work.WorkId);
            return ValueTask.FromResult(new PollingWorkExecutionResult(true));
        }
    }

    private sealed class RecordingPriorityRunner : IPriorityWorkRunner
    {
        private readonly SupervisionRuntimeComposition _composition;
        private readonly SupervisionOperationalFacade _facade;
        private readonly List<BusWorkKind> _sequence;

        public RecordingPriorityRunner(
            SupervisionRuntimeComposition composition,
            SupervisionOperationalFacade facade,
            List<BusWorkKind> sequence)
        {
            _composition = composition;
            _facade = facade;
            _sequence = sequence;
        }

        public List<TR2.Protocol.B5CommandRequest> Requests { get; } = [];

        public ValueTask ExecuteAsync(
            ScheduledBusWork work,
            DateTimeOffset observedAt,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            _sequence.Add(work.Kind);
            Requests.Add(_facade.GetCommandRequest(work));
            _composition.BusWorkScheduler.Complete(work.Endpoint.Bus, work.WorkId);
            return ValueTask.CompletedTask;
        }
    }
}
