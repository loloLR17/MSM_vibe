using System.Collections.Concurrent;
using TR2.Application;
using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class SupervisionPollingLoopTests
{
    [Fact]
    public void RuntimeConfigurationLoadsPollingAndReconnectPoliciesAndRejectsNonPositiveCadence()
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
              "reconnect": {
                "intervalMilliseconds": 250
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
        Assert.NotNull(configuration.Reconnect);
        Assert.Equal(TimeSpan.FromMilliseconds(250), configuration.Reconnect.Interval);

        Assert.Throws<InvalidDataException>(() => RuntimeConfigurationLoader.Parse(
            """
            {
              "persistence": { "databasePath": "tr2.db" },
              "polling": { "fastMilliseconds": 0 },
              "buses": []
            }
            """,
            Path.GetTempPath()));

        Assert.Throws<InvalidDataException>(() => RuntimeConfigurationLoader.Parse(
            """
            {
              "persistence": { "databasePath": "tr2.db" },
              "reconnect": { "intervalMilliseconds": 0 },
              "buses": []
            }
            """,
            Path.GetTempPath()));

        var withoutReconnect = RuntimeConfigurationLoader.Parse(
            """
            {
              "persistence": { "databasePath": "tr2.db" },
              "buses": []
            }
            """,
            Path.GetTempPath());

        Assert.Null(withoutReconnect.Reconnect);
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

    [Fact]
    public async Task RuntimeLoopUsesConfiguredReconnectCadenceForDisconnectedSerialBus()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var factory = new FakeBusConnectionFactory();
            var composition = ComposeSerial(databasePath, factory);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            Assert.Single(factory.OpenRequests);
            await composition.BusConnectionManager.CloseAsync("bus-1");

            var runner = new RecordingPollingRunner(composition);
            var loop = new SupervisionPollingLoop(composition, runner);
            using var cancellation = new CancellationTokenSource();

            var runTask = loop.RunAsync(cancellation.Token);

            await Task.Delay(10);
            Assert.Single(factory.OpenRequests);

            await WaitForAsync(() => factory.OpenRequests.Count >= 2);
            Assert.Equal(2, factory.OpenRequests.Count);
            Assert.True(composition.BusConnectionManager.TryGet("bus-1", out _));

            cancellation.Cancel();
            await Assert.ThrowsAnyAsync<OperationCanceledException>(async () => await runTask);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    private static SupervisionRuntimeComposition Compose(string databasePath)
    {
        var escapedDatabasePath = databasePath.Replace("\\", "\\\\");
        var configuration = RuntimeConfigurationLoader.Parse(
            $$"""
            {
              "persistence": { "databasePath": "{{escapedDatabasePath}}" },
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

    private static SupervisionRuntimeComposition ComposeSerial(
        string databasePath,
        IModbusBusConnectionFactory factory)
    {
        var escapedDatabasePath = databasePath.Replace("\\", "\\\\");
        var configuration = RuntimeConfigurationLoader.Parse(
            $$"""
            {
              "persistence": { "databasePath": "{{escapedDatabasePath}}" },
              "polling": {
                "staticRetryMilliseconds": 100,
                "fastMilliseconds": 100,
                "mediumMilliseconds": 100,
                "slowMilliseconds": 100,
                "scanMilliseconds": 5
              },
              "reconnect": {
                "intervalMilliseconds": 40
              },
              "buses": [
                {
                  "id": "bus-1",
                  "serial": {
                    "portName": "COM7",
                    "baudRate": 115200,
                    "dataBits": 8,
                    "parity": "None",
                    "stopBits": "One",
                    "responseTimeoutMilliseconds": 750
                  },
                  "endpoints": [1]
                }
              ]
            }
            """,
            Path.GetDirectoryName(databasePath)!);

        return SupervisionRuntimeCompositionRoot.Compose(configuration, factory);
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

    private sealed class FakeBusConnectionFactory : IModbusBusConnectionFactory
    {
        public List<(string BusId, ModbusSerialConnectionSettings Settings)> OpenRequests { get; } = [];

        public ValueTask<IModbusBusConnection> OpenAsync(
            string busId,
            ModbusSerialConnectionSettings settings,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            OpenRequests.Add((busId, settings));
            return ValueTask.FromResult<IModbusBusConnection>(new FakeBusConnection(busId));
        }
    }

    private sealed class FakeBusConnection : IModbusBusConnection
    {
        public FakeBusConnection(string busId)
        {
            BusId = busId;
            var transport = new NullRegisterTransport();
            RegisterTransport = transport;
            RegisterWriteTransport = transport;
        }

        public string BusId { get; }
        public IRegisterTransport RegisterTransport { get; }
        public IRegisterWriteTransport RegisterWriteTransport { get; }

        public ValueTask DisposeAsync() => ValueTask.CompletedTask;
    }

    private sealed class NullRegisterTransport : IRegisterTransport, IRegisterWriteTransport
    {
        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromResult(Array.Empty<ushort>());

        public ValueTask WriteRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            IReadOnlyList<ushort> values,
            CancellationToken cancellationToken = default) =>
            ValueTask.CompletedTask;
    }
}
