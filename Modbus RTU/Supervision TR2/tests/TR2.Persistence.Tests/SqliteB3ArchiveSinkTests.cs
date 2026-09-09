using System.Globalization;
using Microsoft.Data.Sqlite;
using TR2.Application;
using TR2.Domain;
using TR2.Persistence.Sqlite;
using TR2.Protocol;
using Xunit;

namespace TR2.Persistence.Tests;

public sealed class SqliteB3ArchiveSinkTests : IDisposable
{
    private readonly string _directory = Path.Combine(Path.GetTempPath(), $"tr2-b3-persistence-{Guid.NewGuid():N}");

    [Fact]
    public async Task AppendAsync_PersistsB3AndB2ContextAcrossReopen()
    {
        var databasePath = Path.Combine(_directory, "b3-reopen.db");
        var database = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        var sink = new SqliteB3ArchiveSink(database);
        var receivedAt = new DateTimeOffset(2026, 9, 9, 15, 0, 0, TimeSpan.FromHours(2));
        var b2ReceivedAt = receivedAt.AddSeconds(-2);

        await sink.AppendAsync(new B3ArchiveObservation(
            new DeviceId(0x12345678),
            CreateB3(calculationSequence: 1234, rmsGlobalMg: 250, alarmCount: 9),
            receivedAt,
            new B3ArchiveTimeContext(CreateB2(), b2ReceivedAt)));

        SqliteConnection.ClearAllPools();
        var reopened = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        using var connection = reopened.OpenConnection();
        using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT device_id, received_utc, calculation_sequence, rms_global_mg, alarm_count,
                   b2_received_utc, b2_current_time_seconds, b2_drift_ppm
            FROM b3_archive;
            """;
        using var reader = command.ExecuteReader();

        Assert.True(reader.Read());
        Assert.Equal(0x12345678L, reader.GetInt64(0));
        Assert.Equal(receivedAt.UtcDateTime, ParseUtc(reader.GetString(1)).UtcDateTime);
        Assert.Equal(1234L, reader.GetInt64(2));
        Assert.Equal(250L, reader.GetInt64(3));
        Assert.Equal(9L, reader.GetInt64(4));
        Assert.Equal(b2ReceivedAt.UtcDateTime, ParseUtc(reader.GetString(5)).UtcDateTime);
        Assert.Equal(1_700_000_000L, reader.GetInt64(6));
        Assert.Equal(-7L, reader.GetInt64(7));
        Assert.False(reader.Read());
    }

    [Fact]
    public async Task AppendAsync_IsAppendOnlyAndAllowsMissingB2Context()
    {
        var database = CreateDatabase("append-only.db");
        var sink = new SqliteB3ArchiveSink(database);
        var deviceId = new DeviceId(77);
        var receivedAt = DateTimeOffset.UtcNow;

        await sink.AppendAsync(new B3ArchiveObservation(deviceId, CreateB3(1, 100, 0), receivedAt));
        await sink.AppendAsync(new B3ArchiveObservation(deviceId, CreateB3(2, 200, 1), receivedAt.AddSeconds(1)));

        using var connection = database.OpenConnection();
        using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT calculation_sequence, rms_global_mg, b2_received_utc
            FROM b3_archive
            WHERE device_id = 77
            ORDER BY observation_id;
            """;
        using var reader = command.ExecuteReader();

        Assert.True(reader.Read());
        Assert.Equal(1L, reader.GetInt64(0));
        Assert.Equal(100L, reader.GetInt64(1));
        Assert.True(reader.IsDBNull(2));
        Assert.True(reader.Read());
        Assert.Equal(2L, reader.GetInt64(0));
        Assert.Equal(200L, reader.GetInt64(1));
        Assert.True(reader.IsDBNull(2));
        Assert.False(reader.Read());
    }

    [Fact]
    public void Database_MigratesVersion2ToB3Schema()
    {
        var databasePath = Path.Combine(_directory, "migration-v2.db");
        Directory.CreateDirectory(_directory);
        using (var connection = new SqliteConnection($"Data Source={databasePath}"))
        {
            connection.Open();
            using var command = connection.CreateCommand();
            command.CommandText = "PRAGMA user_version = 2;";
            command.ExecuteNonQuery();
        }

        var database = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        using var migrated = database.OpenConnection();

        Assert.Equal(SqliteDatabase.CurrentSchemaVersion, ReadInt32(migrated, "PRAGMA user_version;"));
        Assert.Equal(1, ReadInt32(migrated, "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='b3_archive';"));
    }

    public void Dispose()
    {
        SqliteConnection.ClearAllPools();
        if (Directory.Exists(_directory))
            Directory.Delete(_directory, recursive: true);
    }

    private SqliteDatabase CreateDatabase(string fileName) =>
        new(new SqlitePersistenceOptions(Path.Combine(_directory, fileName)));

    private static B3VibrationSupervision CreateB3(uint calculationSequence, uint rmsGlobalMg, uint alarmCount) =>
        new(
            StatusGlobal: 1,
            ValidityFlags: 2,
            AlarmFlags: 3,
            SeverityGlobal: 4,
            LastUpdateTr2Seconds: 100,
            ValueAgeMilliseconds: 20,
            CalculationSequence: calculationSequence,
            WindowDurationMilliseconds: 1000,
            ValidSampleCount: 500,
            RmsGlobalMg: rmsGlobalMg,
            PeakGlobalMg: 400,
            RmsXMg: 101,
            RmsYMg: 102,
            RmsZMg: 103,
            PeakXMg: 201,
            PeakYMg: 202,
            PeakZMg: 203,
            DominantAxis: 2,
            ExceedGlobal: 1,
            ExceedX: 0,
            ExceedY: 1,
            ExceedZ: 0,
            AlarmLatched: 1,
            ExceedCount: 8,
            AlarmCount: alarmCount);

    private static B2TimeState CreateB2() =>
        new(
            TimeStatus: 1,
            TimeFlags: 2,
            CurrentTimeSeconds: 1_700_000_000,
            LastSyncTimeSeconds: 1_699_999_900,
            TimeSinceSyncSeconds: 100,
            PreparedTimeSeconds: 0,
            PreparedTimeStatus: 0,
            TimeAccuracyMilliseconds: 25,
            DriftPpm: -7,
            SyncSource: 2);

    private static DateTimeOffset ParseUtc(string text) =>
        DateTimeOffset.ParseExact(text, "O", CultureInfo.InvariantCulture, DateTimeStyles.RoundtripKind);

    private static int ReadInt32(SqliteConnection connection, string sql)
    {
        using var command = connection.CreateCommand();
        command.CommandText = sql;
        return Convert.ToInt32(command.ExecuteScalar(), CultureInfo.InvariantCulture);
    }
}
