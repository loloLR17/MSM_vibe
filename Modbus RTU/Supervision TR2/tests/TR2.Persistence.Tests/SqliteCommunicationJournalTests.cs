using Microsoft.Data.Sqlite;
using TR2.Application;
using TR2.Domain;
using TR2.Persistence.Sqlite;
using Xunit;

namespace TR2.Persistence.Tests;

public sealed class SqliteCommunicationJournalTests : IDisposable
{
    private readonly string _directory = Path.Combine(Path.GetTempPath(), $"tr2-communication-journal-{Guid.NewGuid():N}");

    [Fact]
    public async Task Append_PersistsCompleteFailureAcrossReopen()
    {
        var databasePath = Path.Combine(_directory, "communication-reopen.db");
        var database = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        var sink = new SqliteCommunicationJournalSink(database);
        var observedAt = new DateTimeOffset(2026, 9, 9, 14, 30, 0, TimeSpan.FromHours(2));
        var endpoint = new TR2Endpoint(new SerialBus("rs485-a"), new ModbusAddress(17));

        await sink.AppendAsync(new CommunicationFailureEvent(
            endpoint,
            new DeviceId(0x12345678),
            CommunicationOperation.Polling,
            observedAt,
            "System.TimeoutException",
            "No response from TR2"));

        SqliteConnection.ClearAllPools();

        var reopened = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        using var connection = reopened.OpenConnection();
        using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT bus_id, modbus_address, device_id, operation, observed_utc, exception_type, message
            FROM communication_failure_journal
            ORDER BY failure_id;
            """;
        using var reader = command.ExecuteReader();

        Assert.True(reader.Read());
        Assert.Equal("rs485-a", reader.GetString(0));
        Assert.Equal(17, reader.GetInt32(1));
        Assert.Equal(0x12345678L, reader.GetInt64(2));
        Assert.Equal("Polling", reader.GetString(3));
        Assert.Equal(observedAt.UtcDateTime, DateTimeOffset.Parse(reader.GetString(4)).UtcDateTime);
        Assert.Equal("System.TimeoutException", reader.GetString(5));
        Assert.Equal("No response from TR2", reader.GetString(6));
        Assert.False(reader.Read());
    }

    [Fact]
    public async Task Append_PreservesMissingDeviceIdentityAsNull()
    {
        var database = CreateDatabase("communication-no-device.db");
        var sink = new SqliteCommunicationJournalSink(database);
        var endpoint = new TR2Endpoint(new SerialBus("rs485-b"), new ModbusAddress(3));

        await sink.AppendAsync(new CommunicationFailureEvent(
            endpoint,
            null,
            CommunicationOperation.ExplicitRefresh,
            DateTimeOffset.UtcNow,
            "System.IO.IOException",
            string.Empty));

        using var connection = database.OpenConnection();
        using var command = connection.CreateCommand();
        command.CommandText = "SELECT device_id, operation, message FROM communication_failure_journal;";
        using var reader = command.ExecuteReader();

        Assert.True(reader.Read());
        Assert.True(reader.IsDBNull(0));
        Assert.Equal("ExplicitRefresh", reader.GetString(1));
        Assert.Equal(string.Empty, reader.GetString(2));
    }

    [Fact]
    public async Task Append_IsAppendOnlyAndKeepsFailureOrder()
    {
        var database = CreateDatabase("communication-order.db");
        var sink = new SqliteCommunicationJournalSink(database);
        var endpoint = new TR2Endpoint(new SerialBus("rs485-c"), new ModbusAddress(42));

        await sink.AppendAsync(new CommunicationFailureEvent(
            endpoint,
            new DeviceId(42),
            CommunicationOperation.Polling,
            DateTimeOffset.UtcNow,
            "FirstException",
            "first"));
        await sink.AppendAsync(new CommunicationFailureEvent(
            endpoint,
            new DeviceId(42),
            CommunicationOperation.Polling,
            DateTimeOffset.UtcNow.AddSeconds(1),
            "SecondException",
            "second"));

        using var connection = database.OpenConnection();
        using var command = connection.CreateCommand();
        command.CommandText = "SELECT exception_type FROM communication_failure_journal ORDER BY failure_id ASC;";
        using var reader = command.ExecuteReader();

        Assert.True(reader.Read());
        Assert.Equal("FirstException", reader.GetString(0));
        Assert.True(reader.Read());
        Assert.Equal("SecondException", reader.GetString(0));
        Assert.False(reader.Read());
    }

    [Fact]
    public void Database_MigratesExistingVersion3ToCommunicationJournalSchema()
    {
        var databasePath = Path.Combine(_directory, "migration-v3.db");
        Directory.CreateDirectory(_directory);

        using (var connection = new SqliteConnection($"Data Source={databasePath}"))
        {
            connection.Open();
            using var command = connection.CreateCommand();
            command.CommandText = "PRAGMA user_version = 3;";
            command.ExecuteNonQuery();
        }

        var database = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        using var migrated = database.OpenConnection();

        Assert.Equal(SqliteDatabase.CurrentSchemaVersion, ReadInt32(migrated, "PRAGMA user_version;"));
        Assert.Equal(
            1,
            ReadInt32(
                migrated,
                "SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' AND name = 'communication_failure_journal';"));
    }

    public void Dispose()
    {
        SqliteConnection.ClearAllPools();

        if (Directory.Exists(_directory))
            Directory.Delete(_directory, recursive: true);
    }

    private SqliteDatabase CreateDatabase(string fileName) =>
        new(new SqlitePersistenceOptions(Path.Combine(_directory, fileName)));

    private static int ReadInt32(SqliteConnection connection, string sql)
    {
        using var command = connection.CreateCommand();
        command.CommandText = sql;
        return Convert.ToInt32(command.ExecuteScalar(), System.Globalization.CultureInfo.InvariantCulture);
    }
}
