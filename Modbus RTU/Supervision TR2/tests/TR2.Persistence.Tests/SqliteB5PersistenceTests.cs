using Microsoft.Data.Sqlite;
using TR2.Application;
using TR2.Domain;
using TR2.Persistence.Sqlite;
using Xunit;

namespace TR2.Persistence.Tests;

public sealed class SqliteB5PersistenceTests : IDisposable
{
    private readonly string _directory = Path.Combine(Path.GetTempPath(), $"tr2-b5-persistence-{Guid.NewGuid():N}");

    [Fact]
    public async Task ReservationStore_ReturnsNullWhenDeviceHasNoAllocation()
    {
        var database = CreateDatabase("empty-reservation.db");
        var store = new SqliteCommandTransactionReservationStore(database);

        var result = await store.GetLastAllocatedAsync(new DeviceId(1001));

        Assert.Null(result);
    }

    [Fact]
    public async Task ReservationStore_PersistsLatestAllocationAcrossReopen()
    {
        var databasePath = Path.Combine(_directory, "reservation-reopen.db");
        var firstDatabase = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        var firstStore = new SqliteCommandTransactionReservationStore(firstDatabase);
        var deviceId = new DeviceId(0x12345678);

        await firstStore.PersistAllocationAsync(deviceId, new TransactionId(41));
        await firstStore.PersistAllocationAsync(deviceId, new TransactionId(42));

        SqliteConnection.ClearAllPools();

        var reopenedDatabase = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        var reopenedStore = new SqliteCommandTransactionReservationStore(reopenedDatabase);
        var restored = await reopenedStore.GetLastAllocatedAsync(deviceId);

        Assert.NotNull(restored);
        Assert.Equal((ushort)42, restored.Value.Value);
    }

    [Fact]
    public async Task ReservationStore_KeepsDevicesIndependent()
    {
        var database = CreateDatabase("reservation-devices.db");
        var store = new SqliteCommandTransactionReservationStore(database);
        var firstDevice = new DeviceId(1);
        var secondDevice = new DeviceId(uint.MaxValue);

        await store.PersistAllocationAsync(firstDevice, new TransactionId(7));
        await store.PersistAllocationAsync(secondDevice, new TransactionId(65535));

        Assert.Equal((ushort)7, (await store.GetLastAllocatedAsync(firstDevice))!.Value.Value);
        Assert.Equal((ushort)65535, (await store.GetLastAllocatedAsync(secondDevice))!.Value.Value);
    }

    [Fact]
    public async Task Journal_AppendsInOrderAndSurvivesReopen()
    {
        var databasePath = Path.Combine(_directory, "journal-reopen.db");
        var firstDatabase = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        var firstJournal = new SqliteCommandTransactionJournal(firstDatabase);
        var deviceId = new DeviceId(77);
        var transactionId = new TransactionId(12);
        var preparedAt = new DateTimeOffset(2026, 9, 9, 12, 0, 0, TimeSpan.FromHours(2));
        var submittedAt = preparedAt.AddMilliseconds(250);

        await firstJournal.AppendAsync(new CommandTransactionJournalEvent(
            deviceId,
            transactionId,
            "request-12",
            CommandTransactionJournalEventKind.Prepared,
            preparedAt));
        await firstJournal.AppendAsync(new CommandTransactionJournalEvent(
            deviceId,
            transactionId,
            "request-12",
            CommandTransactionJournalEventKind.Submitted,
            submittedAt));

        SqliteConnection.ClearAllPools();

        var reopenedDatabase = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        var reopenedJournal = new SqliteCommandTransactionJournal(reopenedDatabase);
        var entries = await reopenedJournal.ReadAsync(deviceId);

        Assert.Collection(
            entries,
            entry => AssertJournalEntry(entry, deviceId, transactionId, "request-12", CommandTransactionJournalEventKind.Prepared, preparedAt),
            entry => AssertJournalEntry(entry, deviceId, transactionId, "request-12", CommandTransactionJournalEventKind.Submitted, submittedAt));
    }

    [Fact]
    public async Task Journal_ReadIsIsolatedByDevice()
    {
        var database = CreateDatabase("journal-devices.db");
        var journal = new SqliteCommandTransactionJournal(database);
        var firstDevice = new DeviceId(10);
        var secondDevice = new DeviceId(20);
        var observedAt = DateTimeOffset.UtcNow;

        await journal.AppendAsync(new CommandTransactionJournalEvent(
            firstDevice,
            new TransactionId(1),
            "first",
            CommandTransactionJournalEventKind.Prepared,
            observedAt));
        await journal.AppendAsync(new CommandTransactionJournalEvent(
            secondDevice,
            new TransactionId(2),
            "second",
            CommandTransactionJournalEventKind.Ambiguous,
            observedAt));

        var firstEntries = await journal.ReadAsync(firstDevice);
        var secondEntries = await journal.ReadAsync(secondDevice);

        Assert.Single(firstEntries);
        Assert.Equal("first", firstEntries[0].RequestIdentity);
        Assert.Single(secondEntries);
        Assert.Equal("second", secondEntries[0].RequestIdentity);
    }

    [Fact]
    public void Database_MigratesExistingS2BVersion1ToB5Schema()
    {
        var databasePath = Path.Combine(_directory, "migration-v1.db");
        Directory.CreateDirectory(_directory);

        using (var connection = new SqliteConnection($"Data Source={databasePath}"))
        {
            connection.Open();
            using var command = connection.CreateCommand();
            command.CommandText = """
                CREATE TABLE tr2_schema_marker (
                    singleton INTEGER NOT NULL PRIMARY KEY CHECK (singleton = 1),
                    created_utc TEXT NOT NULL
                );
                INSERT INTO tr2_schema_marker(singleton, created_utc)
                VALUES (1, '2026-09-09T00:00:00.0000000+00:00');
                PRAGMA user_version = 1;
                """;
            command.ExecuteNonQuery();
        }

        var database = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        using var migrated = database.OpenConnection();

        Assert.Equal(SqliteDatabase.CurrentSchemaVersion, ReadInt32(migrated, "PRAGMA user_version;"));
        Assert.Equal(1, ReadInt32(migrated, "SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' AND name = 'b5_transaction_reservation';"));
        Assert.Equal(1, ReadInt32(migrated, "SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' AND name = 'b5_transaction_journal';"));
    }

    public void Dispose()
    {
        SqliteConnection.ClearAllPools();

        if (Directory.Exists(_directory))
        {
            Directory.Delete(_directory, recursive: true);
        }
    }

    private SqliteDatabase CreateDatabase(string fileName)
    {
        return new SqliteDatabase(new SqlitePersistenceOptions(Path.Combine(_directory, fileName)));
    }

    private static void AssertJournalEntry(
        CommandTransactionJournalEvent entry,
        DeviceId deviceId,
        TransactionId transactionId,
        string requestIdentity,
        CommandTransactionJournalEventKind kind,
        DateTimeOffset observedAt)
    {
        Assert.Equal(deviceId, entry.DeviceId);
        Assert.Equal(transactionId, entry.TransactionId);
        Assert.Equal(requestIdentity, entry.RequestIdentity);
        Assert.Equal(kind, entry.Kind);
        Assert.Equal(observedAt.UtcDateTime, entry.ObservedAt.UtcDateTime);
    }

    private static int ReadInt32(SqliteConnection connection, string sql)
    {
        using var command = connection.CreateCommand();
        command.CommandText = sql;
        return Convert.ToInt32(command.ExecuteScalar(), System.Globalization.CultureInfo.InvariantCulture);
    }
}
