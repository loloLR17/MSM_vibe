using TR2.Application;
using TR2.Domain;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class SupervisionRuntimeStartupTests
{
    [Fact]
    public async Task StartupCreatesDatabaseAndOpensReadinessGate()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var composition = Compose(databasePath);

            Assert.False(composition.ReadinessGate.IsReady);
            Assert.Throws<InvalidOperationException>(() => composition.ReadinessGate.EnsureReady());

            var startup = new SupervisionRuntimeStartup(composition);
            await startup.StartAsync();

            Assert.True(composition.ReadinessGate.IsReady);
            composition.ReadinessGate.EnsureReady();
            Assert.True(File.Exists(databasePath));
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task StartupRestoresNonterminalCommandAsAmbiguousBeforeReady()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var first = Compose(databasePath);
            await new SupervisionRuntimeStartup(first).StartAsync();

            var deviceId = new DeviceId(42);
            var coordinator = new CommandCoordinator(
                deviceId,
                first.CommandReservationStore,
                first.CommandJournal);

            var prepared = await coordinator.PrepareAsync(
                "request-42",
                new DateTimeOffset(2026, 9, 9, 12, 0, 0, TimeSpan.Zero));
            await coordinator.MarkSubmittedAsync(
                new DateTimeOffset(2026, 9, 9, 12, 0, 1, TimeSpan.Zero));

            var second = Compose(databasePath);
            Assert.False(second.ReadinessGate.IsReady);

            await new SupervisionRuntimeStartup(second).StartAsync();

            Assert.True(second.ReadinessGate.IsReady);
            var recovered = second.CommandCoordinatorRegistry.Get(deviceId).ActiveTransaction;
            Assert.NotNull(recovered);
            Assert.Equal(prepared.TransactionId, recovered.TransactionId);
            Assert.Equal("request-42", recovered.RequestIdentity);
            Assert.Equal(CommandTransactionState.Ambiguous, recovered.State);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task StartupRegistersJournalKnownDeviceWithNoActiveTransactionWhenHistoryIsTerminal()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var first = Compose(databasePath);
            await new SupervisionRuntimeStartup(first).StartAsync();

            var deviceId = new DeviceId(77);
            var coordinator = new CommandCoordinator(
                deviceId,
                first.CommandReservationStore,
                first.CommandJournal);

            var prepared = await coordinator.PrepareAsync(
                "request-77",
                new DateTimeOffset(2026, 9, 9, 13, 0, 0, TimeSpan.Zero));
            await coordinator.MarkSubmittedAsync(
                new DateTimeOffset(2026, 9, 9, 13, 0, 1, TimeSpan.Zero));
            await coordinator.ResolveTerminalAsync(
                prepared.TransactionId,
                new DateTimeOffset(2026, 9, 9, 13, 0, 2, TimeSpan.Zero));

            var second = Compose(databasePath);
            await new SupervisionRuntimeStartup(second).StartAsync();

            var recoveredCoordinator = second.CommandCoordinatorRegistry.Get(deviceId);
            Assert.Null(recoveredCoordinator.ActiveTransaction);
            Assert.True(second.ReadinessGate.IsReady);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task StartupIsOneShot()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var composition = Compose(databasePath);
            var startup = new SupervisionRuntimeStartup(composition);

            await startup.StartAsync();

            await Assert.ThrowsAsync<InvalidOperationException>(async () => await startup.StartAsync());
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
              "buses": [
                { "id": "bus-1", "endpoints": [1] }
              ]
            }
            """,
            Path.GetDirectoryName(databasePath)!);

        return SupervisionRuntimeCompositionRoot.Compose(configuration);
    }

    private static string NewDatabasePath() =>
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s3c-{Guid.NewGuid():N}.db");

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
}
