using TR2.Application;
using TR2.Domain;
using TR2.Protocol;
using TR2.Supervision.Web;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class PhysicalB5RestartWebProjectionTests
{
    [Fact]
    public async Task RestartRecoveryIsAmbiguousInWebThenB0ReconciliationBecomesTerminalWithoutReplay()
    {
        var databasePath = NewDatabasePath();
        try
        {
            const ushort supportedProtocolVersion = 7;
            var firstTransport = new ScriptedTransport(supportedProtocolVersion, deviceId: 42);
            var configuration = CreateConfiguration(databasePath);
            var firstComposition = SupervisionRuntimeCompositionRoot.Compose(
                configuration,
                new FakeFactory(firstTransport));
            await new SupervisionRuntimeStartup(firstComposition).StartAsync();

            var endpoint = firstComposition.Configuration.Buses.Single().Endpoints.Single();
            var deviceId = new DeviceId(42);
            firstComposition.FleetRegistry.SetSession(
                TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));

            var firstOperations = new SupervisionOperationalFacade(firstComposition);
            var submittedAt = new DateTimeOffset(2026, 9, 10, 12, 30, 0, TimeSpan.Zero);
            var queued = await firstOperations.QueueCommandAsync(
                endpoint,
                "restart-web-projection",
                new B5CommandIntent(5, 0, 0, 0, 0),
                submittedAt);

            var firstRunner = new PhysicalB5LifecycleWorkRunner(firstComposition, firstOperations);
            var commandWork = firstComposition.BusWorkScheduler.BeginNext(endpoint.Bus, submittedAt)!;
            Assert.Equal(BusWorkKind.CommandTransaction, commandWork.Kind);
            await firstRunner.ExecuteAsync(commandWork, submittedAt);

            Assert.Equal(2, firstTransport.Writes.Count);
            Assert.Equal(
                CommandTransactionState.Submitted,
                firstComposition.CommandCoordinatorRegistry.Get(deviceId).ActiveTransaction!.State);

            var beforeRestart = await firstComposition.CommandJournal.ReadAsync(deviceId);
            Assert.Equal(2, beforeRestart.Count);
            Assert.Equal(CommandTransactionJournalEventKind.Submitted, beforeRestart[^1].Kind);

            await firstComposition.BusConnectionManager.DisposeAsync();

            var secondTransport = new ScriptedTransport(supportedProtocolVersion, deviceId: 42);
            var terminal = new ushort[B5CommandState.RegisterCount];
            terminal[15] = queued.Request.TransactionId;
            terminal[16] = 4;
            secondTransport.B5Registers = terminal;

            var restartedComposition = SupervisionRuntimeCompositionRoot.Compose(
                configuration,
                new FakeFactory(secondTransport));
            await new SupervisionRuntimeStartup(restartedComposition).StartAsync();

            var recoveredCoordinator = restartedComposition.CommandCoordinatorRegistry.Get(deviceId);
            var recovered = Assert.IsType<CommandTransaction>(recoveredCoordinator.ActiveTransaction);
            Assert.Equal(queued.Request.TransactionId, recovered.TransactionId.Value);
            Assert.Equal(CommandTransactionState.Ambiguous, recovered.State);

            var afterRecoveryJournal = await restartedComposition.CommandJournal.ReadAsync(deviceId);
            Assert.Equal(beforeRestart, afterRecoveryJournal);

            var restartedOperations = new SupervisionOperationalFacade(restartedComposition);
            var restartedRuntime = new PhysicalSupervisionRuntime(
                restartedComposition,
                restartedOperations,
                new SupervisionRuntimeHost(restartedComposition));
            var webProjection = new RuntimeCommandSink(restartedRuntime);

            var ambiguousHistory = await webProjection.ReadAsync(deviceId.Value);
            var ambiguousEntry = Assert.Single(ambiguousHistory);
            Assert.Equal(IhmB5TransactionState.Ambiguous, ambiguousEntry.State);
            Assert.Equal(queued.Request.TransactionId, ambiguousEntry.TransactionId);

            var priorityRunner = new PhysicalB5LifecycleWorkRunner(
                restartedComposition,
                restartedOperations);
            var pollingRunner = new PhysicalPollingWorkRunner(
                restartedComposition,
                supportedProtocolVersion,
                priorityRunner.ObserveCompatibleSession);

            var b0At = submittedAt.AddMinutes(1);
            restartedComposition.BusWorkScheduler.QueuePolling(
                endpoint,
                PollingGroup.Static,
                b0At);
            var b0Work = restartedComposition.BusWorkScheduler.BeginNext(endpoint.Bus, b0At)!;
            Assert.Equal(BusWorkKind.Polling, b0Work.Kind);
            Assert.Equal(PollingGroup.Static, b0Work.PollingGroup);

            var b0Result = await pollingRunner.ExecuteAsync(b0Work, b0At);
            Assert.True(b0Result.EndpointCompatible);
            Assert.Equal(deviceId, restartedComposition.FleetRegistry.GetSession(endpoint).Device!.DeviceId);

            var reconcileAt = b0At + configuration.B5!.ReconciliationInterval;
            var reconciliationWork = restartedComposition.BusWorkScheduler.BeginNext(endpoint.Bus, reconcileAt)!;
            Assert.Equal(BusWorkKind.TransactionReconciliation, reconciliationWork.Kind);
            await priorityRunner.ExecuteAsync(reconciliationWork, reconcileAt);

            Assert.Null(recoveredCoordinator.ActiveTransaction);
            Assert.Empty(secondTransport.Writes);
            Assert.Equal(2, firstTransport.Writes.Count);

            var terminalHistory = await webProjection.ReadAsync(deviceId.Value);
            var terminalEntry = Assert.Single(terminalHistory);
            Assert.Equal(IhmB5TransactionState.TerminalEvidenceObserved, terminalEntry.State);
            Assert.Equal(queued.Request.TransactionId, terminalEntry.TransactionId);

            var finalJournal = await restartedComposition.CommandJournal.ReadAsync(deviceId);
            Assert.Equal(3, finalJournal.Count);
            Assert.Equal(CommandTransactionJournalEventKind.TerminalEvidenceObserved, finalJournal[^1].Kind);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    private static RuntimeConfiguration CreateConfiguration(string databasePath) =>
        RuntimeConfigurationLoader.Parse(
            $$"""
            {
              "persistence": { "databasePath": "{{databasePath.Replace("\\", "\\\\")}}" },
              "b5": {
                "postSubmitPollIntervalMilliseconds": 10,
                "postSubmitTimeoutMilliseconds": 50,
                "reconciliationIntervalMilliseconds": 10
              },
              "buses": [{
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
              }]
            }
            """,
            Path.GetDirectoryName(databasePath)!);

    private static string NewDatabasePath() =>
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-h3c-restart-web-{Guid.NewGuid():N}.db");

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

    private sealed class FakeFactory(ScriptedTransport transport) : IModbusBusConnectionFactory
    {
        public ValueTask<IModbusBusConnection> OpenAsync(
            string busId,
            ModbusSerialConnectionSettings settings,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            return ValueTask.FromResult<IModbusBusConnection>(new FakeConnection(busId, transport));
        }
    }

    private sealed class FakeConnection(string busId, ScriptedTransport transport) : IModbusBusConnection
    {
        public string BusId => busId;
        public IRegisterTransport RegisterTransport => transport;
        public IRegisterWriteTransport RegisterWriteTransport => transport;
        public ValueTask DisposeAsync() => ValueTask.CompletedTask;
    }

    private sealed class ScriptedTransport : IRegisterTransport, IRegisterWriteTransport
    {
        private readonly ushort[] _b0Registers = new ushort[21];

        public ScriptedTransport(ushort supportedProtocolVersion, ushort deviceId)
        {
            _b0Registers[1] = deviceId;
            _b0Registers[6] = supportedProtocolVersion;
        }

        public ushort[] B5Registers { get; set; } = new ushort[B5CommandState.RegisterCount];
        public List<(ushort StartAddress, ushort[] Values)> Writes { get; } = [];

        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();

            if (startAddress == 0)
            {
                return ValueTask.FromResult(_b0Registers.Take(registerCount).ToArray());
            }

            if (startAddress == B5CommandState.StartAddress)
            {
                return ValueTask.FromResult(B5Registers.Take(registerCount).ToArray());
            }

            return ValueTask.FromResult(new ushort[registerCount]);
        }

        public ValueTask WriteRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            IReadOnlyList<ushort> values,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            Writes.Add((startAddress, values.ToArray()));
            return ValueTask.CompletedTask;
        }
    }
}
