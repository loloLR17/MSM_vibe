using System.Collections.Concurrent;
using TR2.Application;
using TR2.Domain;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class SupervisionPollingLoopTests
{
    [Fact]
    public void RuntimeConfigurationLoadsPollingPolicyAndRejectsNonPositiveCadence()
    {
        var configuration = RuntimeConfigurationLoader.Parse(
            """
            {
              "persistence": { "databasePath": "tr2.db" },
              "polling": {
                "staticRetryMilliseconds": 40,
                "fastMilliseconds": 20,
                "mediumMilliseconds": 60,
                "slowMilliseconds": 120,
                "scanMilliseconds": 5
              },
              "buses": []
            }
            """,
            Path.GetTempPath());

        Assert.Equal(TimeSpan.FromMilliseconds(40), configuration.Polling.StaticRetryInterval);
        Assert.Equal(TimeSpan.FromMilliseconds(20), configuration.Polling.FastInterval);
        Assert.Equal(TimeSpan.FromMilliseconds(60), configuration.Polling.MediumInterval);
        Assert.Equal(TimeSpan.FromMilliseconds(120), configuration.Polling.SlowInterval);
        Assert.Equal(TimeSpan.FromMilliseconds(5), configuration.Polling.ScanInterval);

        Assert.Throws<InvalidDataException>(() => RuntimeConfigurationLoader.Parse(
            """
            {
              "persistence": { "databasePath": "tr2.db" },
              "polling": { "fastMilliseconds": 0 },
              "buses": []
            }
            """,
            Path.GetTempPath()));
    }

    [Fact]
    public async Task PollingLoopRequiresReadiness()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var composition = Compose(databasePath);
            var runner = new RecordingPollingRunner(composition);
            var loop = new SupervisionPollingLoop(composition, runner);

            await Assert.ThrowsAsync<InvalidOperationException>(async () => await loop.RunAsync());
            Assert.Empty(runner.Groups);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task RuntimePollsStaticBeforeOperationalGroupsAndStopsWithHostCancellation()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var composition = Compose(databasePath);
            var runner = new RecordingPollingRunner(composition);
            var loop = new SupervisionPollingLoop(composition, runner);
            var host = new SupervisionRuntimeHost(composition, loop);
            using var cancellation = new CancellationTokenSource();

            var runTask = host.RunAsync(cancellation.Token);
            await WaitForAsync(() => runner.Groups.Count >= 2);

            var groups = runner.Groups.ToArray();
            Assert.Equal(PollingGroup.Static, groups[0]);
            Assert.Contains(PollingGroup.Fast, groups);
            Assert.Equal(SupervisionRuntimeState.Running, host.State);
            Assert.True(composition.ReadinessGate.IsReady);

            cancellation.Cancel();
            await runTask;

            Assert.Equal(SupervisionRuntimeState.Stopped, host.State);
            Assert.False(composition.ReadinessGate.IsReady);
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
              "persistence": { "databasePath": "{{databasePath.Replace("\", "\\")}}" },
              "polling": {
                "staticRetryMilliseconds": 10,
                "fastMilliseconds": 20,
                "mediumMilliseconds": 100,
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
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s3e-{Guid.NewGuid():N}.db");

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

        public RecordingPollingRunner(SupervisionRuntimeComposition composition)
        {
            _composition = composition;
        }

        public ConcurrentQueue<PollingGroup> Groups { get; } = new();

        public ValueTask<PollingWorkExecutionResult> ExecuteAsync(
            ScheduledBusWork work,
            DateTimeOffset observedAt,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            Groups.Enqueue(work.PollingGroup!.Value);

            try
            {
                if (work.PollingGroup == PollingGroup.Static)
                {
                    _composition.FleetRegistry.SetSession(
                        TR2Session.CreateCompatible(work.Endpoint, new TR2Device(new DeviceId(1001))));
                }

                return ValueTask.FromResult(new PollingWorkExecutionResult(true));
            }
            finally
            {
                _composition.BusWorkScheduler.Complete(work.Endpoint.Bus, work.WorkId);
            }
        }
    }
}
