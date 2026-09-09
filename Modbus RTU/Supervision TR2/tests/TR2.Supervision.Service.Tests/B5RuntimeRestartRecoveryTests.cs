using Microsoft.Data.Sqlite;
using TR2.Application;
using TR2.Domain;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class B5RuntimeRestartRecoveryTests
{
    [Fact]
    public async Task RestartRecoversPreparedAsAmbiguousBlocksNewCommandAndDoesNotReplayJournal()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var configuration = CreateConfiguration(databasePath);
            var firstComposition = SupervisionRuntimeCompositionRoot.Compose(configuration);
            var deviceId = new DeviceId(42);
            var firstCoordinator = new CommandCoordinator(
                deviceId,
                firstComposition.CommandReservationStore,
                firstComposition.CommandJournal);

            var prepared = await firstCoordinator.PrepareAsync(
                "request-a",
                new DateTimeOffset(2026, 9, 9, 19, 0, 0, TimeSpan.Zero));

            Assert.Equal(CommandTransactionState.Prepared, prepared.State);
            var beforeRestart = await firstComposition.CommandJournal.ReadAsync(deviceId);
            Assert.Single(beforeRestart);
            Assert.Equal(CommandTransactionJournalEventKind.Prepared, beforeRestart[0].Kind);

            var restartedComposition = SupervisionRuntimeCompositionRoot.Compose(configuration);
            var startup = new SupervisionRuntimeStartup(restartedComposition);
            await startup.StartAsync();

            var recoveredCoordinator = restartedComposition.CommandCoordinatorRegistry.Get(deviceId);
            var recovered = Assert.IsType<CommandTransaction>(recoveredCoordinator.ActiveTransaction);
            Assert.Equal(prepared.TransactionId, recovered.TransactionId);
            Assert.Equal(prepared.RequestIdentity, recovered.RequestIdentity);
            Assert.Equal(CommandTransactionState.Ambiguous, recovered.State);

            await Assert.ThrowsAsync<InvalidOperationException>(async () =>
                await recoveredCoordinator.PrepareAsync(
                    "request-b",
                    new DateTimeOffset(2026, 9, 9, 19, 1, 0, TimeSpan.Zero)));

            var afterRestart = await restartedComposition.CommandJournal.ReadAsync(deviceId);
            Assert.Single(afterRestart);
            Assert.Equal(CommandTransactionJournalEventKind.Prepared, afterRestart[0].Kind);
            Assert.Equal(prepared.TransactionId, afterRestart[0].TransactionId);
            Assert.Equal(prepared.RequestIdentity, afterRestart[0].RequestIdentity);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task RestartPreservesAlreadyAmbiguousTransactionWithoutAddingJournalEvents()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var configuration = CreateConfiguration(databasePath);
            var firstComposition = SupervisionRuntimeCompositionRoot.Compose(configuration);
            var deviceId = new DeviceId(77);
            var coordinator = new CommandCoordinator(
                deviceId,
                firstComposition.CommandReservationStore,
                firstComposition.CommandJournal);

            var prepared = await coordinator.PrepareAsync(
                "request-ambiguous",
                new DateTimeOffset(2026, 9, 9, 20, 0, 0, TimeSpan.Zero));
            await coordinator.MarkSubmittedAsync(
                new DateTimeOffset(2026, 9, 9, 20, 0, 1, TimeSpan.Zero));
            var ambiguous = await coordinator.MarkAmbiguousAfterSubmitAttemptAsync(
                new DateTimeOffset(2026, 9, 9, 20, 0, 2, TimeSpan.Zero));

            Assert.Equal(CommandTransactionState.Ambiguous, ambiguous.State);
            var beforeRestart = await firstComposition.CommandJournal.ReadAsync(deviceId);
            Assert.Equal(3, beforeRestart.Count);

            var restartedComposition = SupervisionRuntimeCompositionRoot.Compose(configuration);
            await new SupervisionRuntimeStartup(restartedComposition).StartAsync();

            var recovered = Assert.IsType<CommandTransaction>(
                restartedComposition.CommandCoordinatorRegistry.Get(deviceId).ActiveTransaction);
            Assert.Equal(prepared.TransactionId, recovered.TransactionId);
            Assert.Equal(CommandTransactionState.Ambiguous, recovered.State);

            var afterRestart = await restartedComposition.CommandJournal.ReadAsync(deviceId);
            Assert.Equal(beforeRestart, afterRestart);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task RestartAfterTerminalHistoryAllowsNextTransactionIdWithoutReuseOrReplay()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var configuration = CreateConfiguration(databasePath);
            var firstComposition = SupervisionRuntimeCompositionRoot.Compose(configuration);
            var deviceId = new DeviceId(91);
            var firstCoordinator = new CommandCoordinator(
                deviceId,
                firstComposition.CommandReservationStore,
                firstComposition.CommandJournal);
            var startedAt = new DateTimeOffset(2026, 9, 9, 21, 0, 0, TimeSpan.Zero);

            var first = await firstCoordinator.PrepareAsync("request-terminal", startedAt);
            await firstCoordinator.MarkSubmittedAsync(startedAt.AddSeconds(1));
            await firstCoordinator.ResolveTerminalAsync(first.TransactionId, startedAt.AddSeconds(2));

            Assert.Null(firstCoordinator.ActiveTransaction);
            var beforeRestart = await firstComposition.CommandJournal.ReadAsync(deviceId);
            Assert.Equal(3, beforeRestart.Count);
            Assert.Equal(CommandTransactionJournalEventKind.TerminalEvidenceObserved, beforeRestart[^1].Kind);

            var restartedComposition = SupervisionRuntimeCompositionRoot.Compose(configuration);
            await new SupervisionRuntimeStartup(restartedComposition).StartAsync();

            var recoveredCoordinator = restartedComposition.CommandCoordinatorRegistry.Get(deviceId);
            Assert.Null(recoveredCoordinator.ActiveTransaction);

            var afterRestart = await restartedComposition.CommandJournal.ReadAsync(deviceId);
            Assert.Equal(beforeRestart, afterRestart);

            var next = await recoveredCoordinator.PrepareAsync(
                "request-after-restart",
                startedAt.AddMinutes(1));

            Assert.Equal((ushort)(first.TransactionId.Value + 1), next.TransactionId.Value);
            Assert.NotEqual(first.TransactionId, next.TransactionId);
            Assert.Equal(CommandTransactionState.Prepared, next.State);

            var afterNewPrepare = await restartedComposition.CommandJournal.ReadAsync(deviceId);
            Assert.Equal(beforeRestart.Count + 1, afterNewPrepare.Count);
            Assert.Equal(CommandTransactionJournalEventKind.Prepared, afterNewPrepare[^1].Kind);
            Assert.Equal(next.TransactionId, afterNewPrepare[^1].TransactionId);
            Assert.Equal("request-after-restart", afterNewPrepare[^1].RequestIdentity);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task RestartRecoversMultipleDevicesIndependentlyWithoutCrossContaminationOrReplay()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var configuration = CreateConfiguration(databasePath);
            var firstComposition = SupervisionRuntimeCompositionRoot.Compose(configuration);
            var terminalDevice = new DeviceId(101);
            var preparedDevice = new DeviceId(102);
            var ambiguousDevice = new DeviceId(103);
            var startedAt = new DateTimeOffset(2026, 9, 9, 22, 0, 0, TimeSpan.Zero);

            var terminalCoordinator = new CommandCoordinator(
                terminalDevice,
                firstComposition.CommandReservationStore,
                firstComposition.CommandJournal);
            var terminal = await terminalCoordinator.PrepareAsync("terminal-request", startedAt);
            await terminalCoordinator.MarkSubmittedAsync(startedAt.AddSeconds(1));
            await terminalCoordinator.ResolveTerminalAsync(terminal.TransactionId, startedAt.AddSeconds(2));

            var preparedCoordinator = new CommandCoordinator(
                preparedDevice,
                firstComposition.CommandReservationStore,
                firstComposition.CommandJournal);
            var prepared = await preparedCoordinator.PrepareAsync(
                "prepared-request",
                startedAt.AddMinutes(1));

            var ambiguousCoordinator = new CommandCoordinator(
                ambiguousDevice,
                firstComposition.CommandReservationStore,
                firstComposition.CommandJournal);
            var ambiguousPrepared = await ambiguousCoordinator.PrepareAsync(
                "ambiguous-request",
                startedAt.AddMinutes(2));
            await ambiguousCoordinator.MarkSubmittedAsync(startedAt.AddMinutes(2).AddSeconds(1));
            await ambiguousCoordinator.MarkAmbiguousAfterSubmitAttemptAsync(
                startedAt.AddMinutes(2).AddSeconds(2));

            var terminalBefore = await firstComposition.CommandJournal.ReadAsync(terminalDevice);
            var preparedBefore = await firstComposition.CommandJournal.ReadAsync(preparedDevice);
            var ambiguousBefore = await firstComposition.CommandJournal.ReadAsync(ambiguousDevice);

            var restartedComposition = SupervisionRuntimeCompositionRoot.Compose(configuration);
            await new SupervisionRuntimeStartup(restartedComposition).StartAsync();

            Assert.Equal(3, restartedComposition.CommandCoordinatorRegistry.Coordinators.Count);

            var recoveredTerminal = restartedComposition.CommandCoordinatorRegistry.Get(terminalDevice);
            Assert.Null(recoveredTerminal.ActiveTransaction);

            var recoveredPrepared = Assert.IsType<CommandTransaction>(
                restartedComposition.CommandCoordinatorRegistry.Get(preparedDevice).ActiveTransaction);
            Assert.Equal(prepared.TransactionId, recoveredPrepared.TransactionId);
            Assert.Equal("prepared-request", recoveredPrepared.RequestIdentity);
            Assert.Equal(CommandTransactionState.Ambiguous, recoveredPrepared.State);

            var recoveredAmbiguous = Assert.IsType<CommandTransaction>(
                restartedComposition.CommandCoordinatorRegistry.Get(ambiguousDevice).ActiveTransaction);
            Assert.Equal(ambiguousPrepared.TransactionId, recoveredAmbiguous.TransactionId);
            Assert.Equal("ambiguous-request", recoveredAmbiguous.RequestIdentity);
            Assert.Equal(CommandTransactionState.Ambiguous, recoveredAmbiguous.State);

            Assert.Equal(terminalBefore, await restartedComposition.CommandJournal.ReadAsync(terminalDevice));
            Assert.Equal(preparedBefore, await restartedComposition.CommandJournal.ReadAsync(preparedDevice));
            Assert.Equal(ambiguousBefore, await restartedComposition.CommandJournal.ReadAsync(ambiguousDevice));

            await Assert.ThrowsAsync<InvalidOperationException>(async () =>
                await restartedComposition.CommandCoordinatorRegistry.Get(preparedDevice).PrepareAsync(
                    "blocked-after-restart",
                    startedAt.AddMinutes(3)));

            var nextTerminal = await recoveredTerminal.PrepareAsync(
                "terminal-next",
                startedAt.AddMinutes(4));
            Assert.Equal((ushort)(terminal.TransactionId.Value + 1), nextTerminal.TransactionId.Value);
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
              "buses": []
            }
            """,
            Path.GetDirectoryName(databasePath)!);

    private static string NewDatabasePath() =>
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s5e-{Guid.NewGuid():N}.db");

    private static void DeleteDatabaseFiles(string databasePath)
    {
        SqliteConnection.ClearAllPools();
        foreach (var suffix in new[] { string.Empty, "-wal", "-shm" })
        {
            var path = databasePath + suffix;
            if (File.Exists(path))
            {
                File.Delete(path);
            }
        }
    }
}
