using System.Globalization;
using Microsoft.Data.Sqlite;

namespace TR2.Persistence.Sqlite;

public sealed class SqliteDatabase
{
    public const int CurrentSchemaVersion = 3;

    private readonly SqlitePersistenceOptions _options;

    public SqliteDatabase(SqlitePersistenceOptions options)
    {
        _options = options ?? throw new ArgumentNullException(nameof(options));
    }

    public SqliteConnection OpenConnection()
    {
        EnsureParentDirectory();

        var builder = new SqliteConnectionStringBuilder
        {
            DataSource = _options.DatabasePath,
            Mode = SqliteOpenMode.ReadWriteCreate,
            ForeignKeys = true,
            Pooling = true
        };

        var connection = new SqliteConnection(builder.ToString());
        connection.Open();

        try
        {
            ConfigureConnection(connection);
            ApplyMigrations(connection);
            return connection;
        }
        catch
        {
            connection.Dispose();
            throw;
        }
    }

    private void EnsureParentDirectory()
    {
        var directory = Path.GetDirectoryName(_options.DatabasePath);
        if (!string.IsNullOrEmpty(directory))
        {
            Directory.CreateDirectory(directory);
        }
    }

    private void ConfigureConnection(SqliteConnection connection)
    {
        using (var command = connection.CreateCommand())
        {
            command.CommandText = "PRAGMA journal_mode = WAL;";
            var journalMode = Convert.ToString(command.ExecuteScalar(), CultureInfo.InvariantCulture);
            if (!string.Equals(journalMode, "wal", StringComparison.OrdinalIgnoreCase))
            {
                throw new InvalidOperationException($"SQLite WAL mode could not be enabled. Returned mode: '{journalMode ?? "<null>"}'.");
            }
        }

        ExecuteNonQuery(connection, "PRAGMA synchronous = FULL;");
        ExecuteNonQuery(connection, "PRAGMA foreign_keys = ON;");
        ExecuteNonQuery(connection, $"PRAGMA busy_timeout = {_options.BusyTimeoutMilliseconds.ToString(CultureInfo.InvariantCulture)};");

        if (ReadInt32(connection, "PRAGMA synchronous;") != 2)
            throw new InvalidOperationException("SQLite synchronous=FULL could not be verified.");
        if (ReadInt32(connection, "PRAGMA foreign_keys;") != 1)
            throw new InvalidOperationException("SQLite foreign_keys=ON could not be verified.");
        if (ReadInt32(connection, "PRAGMA busy_timeout;") != _options.BusyTimeoutMilliseconds)
            throw new InvalidOperationException("SQLite busy_timeout could not be verified.");
    }

    private static void ApplyMigrations(SqliteConnection connection)
    {
        var version = ReadInt32(connection, "PRAGMA user_version;");
        if (version > CurrentSchemaVersion)
            throw new NotSupportedException($"SQLite schema version {version} is newer than supported version {CurrentSchemaVersion}.");

        while (version < CurrentSchemaVersion)
        {
            version = version switch
            {
                0 => ApplyMigration1(connection),
                1 => ApplyMigration2(connection),
                2 => ApplyMigration3(connection),
                _ => throw new InvalidOperationException($"No migration path is defined from schema version {version}.")
            };
        }
    }

    private static int ApplyMigration1(SqliteConnection connection)
    {
        using var transaction = connection.BeginTransaction();
        using (var command = connection.CreateCommand())
        {
            command.Transaction = transaction;
            command.CommandText = """
                CREATE TABLE tr2_schema_marker (
                    singleton INTEGER NOT NULL PRIMARY KEY CHECK (singleton = 1),
                    created_utc TEXT NOT NULL
                );
                INSERT INTO tr2_schema_marker(singleton, created_utc)
                VALUES (1, strftime('%Y-%m-%dT%H:%M:%fZ', 'now'));
                """;
            command.ExecuteNonQuery();
        }
        SetUserVersion(connection, transaction, 1);
        transaction.Commit();
        return 1;
    }

    private static int ApplyMigration2(SqliteConnection connection)
    {
        using var transaction = connection.BeginTransaction();
        using (var command = connection.CreateCommand())
        {
            command.Transaction = transaction;
            command.CommandText = """
                CREATE TABLE b5_transaction_reservation (
                    device_id INTEGER NOT NULL PRIMARY KEY CHECK (device_id BETWEEN 0 AND 4294967295),
                    transaction_id INTEGER NOT NULL CHECK (transaction_id BETWEEN 1 AND 65535),
                    persisted_utc TEXT NOT NULL
                );
                CREATE TABLE b5_transaction_journal (
                    journal_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
                    device_id INTEGER NOT NULL CHECK (device_id BETWEEN 0 AND 4294967295),
                    transaction_id INTEGER NOT NULL CHECK (transaction_id BETWEEN 1 AND 65535),
                    request_identity TEXT NOT NULL CHECK (length(trim(request_identity)) > 0),
                    event_kind TEXT NOT NULL CHECK (event_kind IN ('Prepared', 'Submitted', 'Ambiguous', 'TerminalEvidenceObserved')),
                    observed_utc TEXT NOT NULL
                );
                CREATE INDEX ix_b5_transaction_journal_device_journal
                ON b5_transaction_journal(device_id, journal_id);
                """;
            command.ExecuteNonQuery();
        }
        SetUserVersion(connection, transaction, 2);
        transaction.Commit();
        return 2;
    }

    private static int ApplyMigration3(SqliteConnection connection)
    {
        using var transaction = connection.BeginTransaction();
        using (var command = connection.CreateCommand())
        {
            command.Transaction = transaction;
            command.CommandText = """
                CREATE TABLE b3_archive (
                    observation_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
                    device_id INTEGER NOT NULL CHECK (device_id BETWEEN 0 AND 4294967295),
                    received_utc TEXT NOT NULL,
                    status_global INTEGER NOT NULL,
                    validity_flags INTEGER NOT NULL,
                    alarm_flags INTEGER NOT NULL,
                    severity_global INTEGER NOT NULL,
                    last_update_tr2_seconds INTEGER NOT NULL,
                    value_age_ms INTEGER NOT NULL,
                    calculation_sequence INTEGER NOT NULL,
                    window_duration_ms INTEGER NOT NULL,
                    valid_sample_count INTEGER NOT NULL,
                    rms_global_mg INTEGER NOT NULL,
                    peak_global_mg INTEGER NOT NULL,
                    rms_x_mg INTEGER NOT NULL,
                    rms_y_mg INTEGER NOT NULL,
                    rms_z_mg INTEGER NOT NULL,
                    peak_x_mg INTEGER NOT NULL,
                    peak_y_mg INTEGER NOT NULL,
                    peak_z_mg INTEGER NOT NULL,
                    dominant_axis INTEGER NOT NULL,
                    exceed_global INTEGER NOT NULL,
                    exceed_x INTEGER NOT NULL,
                    exceed_y INTEGER NOT NULL,
                    exceed_z INTEGER NOT NULL,
                    alarm_latched INTEGER NOT NULL,
                    exceed_count INTEGER NOT NULL,
                    alarm_count INTEGER NOT NULL,
                    b2_received_utc TEXT NULL,
                    b2_time_status INTEGER NULL,
                    b2_time_flags INTEGER NULL,
                    b2_current_time_seconds INTEGER NULL,
                    b2_last_sync_time_seconds INTEGER NULL,
                    b2_time_since_sync_seconds INTEGER NULL,
                    b2_prepared_time_seconds INTEGER NULL,
                    b2_prepared_time_status INTEGER NULL,
                    b2_time_accuracy_ms INTEGER NULL,
                    b2_drift_ppm INTEGER NULL,
                    b2_sync_source INTEGER NULL
                );
                CREATE INDEX ix_b3_archive_device_observation
                ON b3_archive(device_id, observation_id);
                """;
            command.ExecuteNonQuery();
        }
        SetUserVersion(connection, transaction, 3);
        transaction.Commit();
        return 3;
    }

    private static void SetUserVersion(SqliteConnection connection, SqliteTransaction transaction, int version)
    {
        using var command = connection.CreateCommand();
        command.Transaction = transaction;
        command.CommandText = $"PRAGMA user_version = {version.ToString(CultureInfo.InvariantCulture)};";
        command.ExecuteNonQuery();
    }

    private static void ExecuteNonQuery(SqliteConnection connection, string sql)
    {
        using var command = connection.CreateCommand();
        command.CommandText = sql;
        command.ExecuteNonQuery();
    }

    private static int ReadInt32(SqliteConnection connection, string sql)
    {
        using var command = connection.CreateCommand();
        command.CommandText = sql;
        return Convert.ToInt32(command.ExecuteScalar(), CultureInfo.InvariantCulture);
    }
}
