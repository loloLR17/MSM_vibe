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
            CommunicationFailureCategory.Timeout,
            observedAt,
            "System.TimeoutException",
            "No response from TR2"));

        SqliteConnection.ClearAllPools();

        var reopened = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        using var connection = reopened.OpenConnection();
        using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT bus_id, modbus_address, device_id, operation, category, observed_utc, exception_type, message
            FROM communication_failure_journal
            ORDER BY failure_id;
            """;
        using var reader = command.ExecuteReader();

        Assert.True(reader.Read());
        Assert.Equal("rs485-a", reader.GetString(0));
        Assert.Equal(17, reader.GetInt32(1));
        Assert.Equal(0x12345678L, reader.GetInt64(2));
        Assert.Equal("Polling", reader.GetString(3));
        Assert.Equal("Timeout", reader.GetString(4));
        Assert.Equal(observedAt.UtcDateTime, DateTimeOffset.Parse(reader.GetString(5)).UtcDateTime);
        Assert.Equal("System.TimeoutException", reader.GetString(6));
        Assert.Equal("No response from TR2", reader.GetString(7));
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
            CommunicationFailureCategory.Io,
            DateTimeOffset.UtcNow,
            "System.IO.IOException",
            string.Empty));

        using var connection = database.OpenConnection();
        using var command = connection.CreateCommand();
        command.CommandText = "SELECT device_id, operation, category, message FROM communication_failure_journal;";
        using var reader = command.ExecuteReader();

        Assert.True(reader.Read());
        Assert.True(reader.IsDBNull(0));
        Assert.Equal("ExplicitRefresh", reader.GetString(1));
        Assert.Equal("Io", reader.GetString(2));
        Assert.Equal(string.Empty, reader.GetString(3));
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
            CommunicationFailureCategory.Unclassified,
            DateTimeOffset.UtcNow,
            "FirstException",
            "first"));
        await sink.AppendAsync(new CommunicationFailureEvent(
            endpoint,
            new DeviceId(42),
            CommunicationOperation.Polling,
            CommunicationFailureCategory.Unclassified,
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

    [Fact]
    public void Database_MigratesVersion5JournalRowsWithoutGuessingHistoricalCategory()
    {
        var databasePath = Path.Combine(_directory, "migration-v5-journal.db");
        Directory.CreateDirectory(_directory);

        using (var connection = new SqliteConnection($"Data Source={databasePath}"))
        {
            connection.Open();
            using var command = connection.CreateCommand();
            command.CommandText = """
                CREATE TABLE communication_failure_journal (
                    failure_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
                    bus_id TEXT NOT NULL CHECK (length(trim(bus_id)) > 0),
                    modbus_address INTEGER NOT NULL CHECK (modbus_address BETWEEN 0 AND 255),
                    device_id INTEGER NULL CHECK (device_id IS NULL OR device_id BETWEEN 0 AND 4294967295),
                    operation TEXT NOT NULL CHECK (operation IN ('Polling', 'ExplicitRefresh')),
                    observed_utc TEXT NOT NULL,
                    exception_type TEXT NOT NULL CHECK (length(trim(exception_type)) > 0),
                    message TEXT NOT NULL
                );
                CREATE INDEX ix_communication_failure_journal_endpoint_failure
                ON communication_failure_journal(bus_id, modbus_address, failure_id);
                CREATE INDEX ix_communication_failure_journal_device_failure
                ON communication_failure_journal(device_id, failure_id)
                WHERE device_id IS NOT NULL;

                CREATE TABLE installation (
                    installation_id TEXT NOT NULL PRIMARY KEY CHECK (length(trim(installation_id)) > 0),
                    name TEXT NOT NULL CHECK (length(trim(name)) > 0)
                );
                CREATE TABLE equipment (
                    equipment_id TEXT NOT NULL PRIMARY KEY CHECK (length(trim(equipment_id)) > 0),
                    installation_id TEXT NOT NULL,
                    name TEXT NOT NULL CHECK (length(trim(name)) > 0),
                    FOREIGN KEY (installation_id) REFERENCES installation(installation_id)
                );
                CREATE TABLE measurement_point (
                    measurement_point_id TEXT NOT NULL PRIMARY KEY CHECK (length(trim(measurement_point_id)) > 0),
                    equipment_id TEXT NOT NULL,
                    name TEXT NOT NULL CHECK (length(trim(name)) > 0),
                    FOREIGN KEY (equipment_id) REFERENCES equipment(equipment_id)
                );
                CREATE TABLE equipment_assignment (
                    assignment_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
                    device_id INTEGER NOT NULL CHECK (device_id BETWEEN 0 AND 4294967295),
                    measurement_point_id TEXT NOT NULL,
                    valid_from_utc TEXT NOT NULL,
                    valid_to_utc TEXT NULL,
                    CHECK (valid_to_utc IS NULL OR valid_to_utc > valid_from_utc),
                    FOREIGN KEY (measurement_point_id) REFERENCES measurement_point(measurement_point_id)
                );

                INSERT INTO communication_failure_journal(
                    bus_id, modbus_address, device_id, operation, observed_utc, exception_type, message)
                VALUES (
                    'rs485-old', 9, 123, 'Polling', '2026-09-09T12:00:00.0000000+00:00',
                    'System.TimeoutException', 'historical timeout');

                PRAGMA user_version = 5;
                """;
            command.ExecuteNonQuery();
        }

        var database = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        using var migrated = database.OpenConnection();
        using var query = migrated.CreateCommand();
        query.CommandText = """
            SELECT operation, category, exception_type, message
            FROM communication_failure_journal
            WHERE bus_id = 'rs485-old' AND modbus_address = 9;
            """;
        using var reader = query.ExecuteReader();

        Assert.True(reader.Read());
        Assert.Equal("Polling", reader.GetString(0));
        Assert.Equal("Unclassified", reader.GetString(1));
        Assert.Equal("System.TimeoutException", reader.GetString(2));
        Assert.Equal("historical timeout", reader.GetString(3));
        Assert.False(reader.Read());
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
