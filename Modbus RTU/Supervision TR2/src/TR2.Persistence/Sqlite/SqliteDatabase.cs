using System.Globalization;
using Microsoft.Data.Sqlite;

namespace TR2.Persistence.Sqlite;

public sealed class SqliteDatabase
{
    public const int CurrentSchemaVersion = 6;

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
            Directory.CreateDirectory(directory);
    }

    private void ConfigureConnection(SqliteConnection connection)
    {
        using (var command = connection.CreateCommand())
        {
            command.CommandText = "PRAGMA journal_mode = WAL;";
            var journalMode = Convert.ToString(command.ExecuteScalar(), CultureInfo.InvariantCulture);
            if (!string.Equals(journalMode, "wal", StringComparison.OrdinalIgnoreCase))
                throw new InvalidOperationException($"SQLite WAL mode could not be enabled. Returned mode: '{journalMode ?? "<null>"}'.");
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
                3 => ApplyMigration4(connection),
                4 => ApplyMigration5(connection),
                5 => ApplyMigration6(connection),
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

    private static int ApplyMigration4(SqliteConnection connection)
    {
        using var transaction = connection.BeginTransaction();
        using (var command = connection.CreateCommand())
        {
            command.Transaction = transaction;
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
                """;
            command.ExecuteNonQuery();
        }
        SetUserVersion(connection, transaction, 4);
        transaction.Commit();
        return 4;
    }

    private static int ApplyMigration5(SqliteConnection connection)
    {
        using var transaction = connection.BeginTransaction();
        using (var command = connection.CreateCommand())
        {
            command.Transaction = transaction;
            command.CommandText = """
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

                CREATE UNIQUE INDEX ux_equipment_assignment_active_device
                ON equipment_assignment(device_id)
                WHERE valid_to_utc IS NULL;

                CREATE UNIQUE INDEX ux_equipment_assignment_active_point
                ON equipment_assignment(measurement_point_id)
                WHERE valid_to_utc IS NULL;

                CREATE INDEX ix_equipment_assignment_device_history
                ON equipment_assignment(device_id, assignment_id);

                CREATE INDEX ix_equipment_assignment_point_history
                ON equipment_assignment(measurement_point_id, assignment_id);
                """;
            command.ExecuteNonQuery();
        }
        SetUserVersion(connection, transaction, 5);
        transaction.Commit();
        return 5;
    }

    private static int ApplyMigration6(SqliteConnection connection)
    {
        using var transaction = connection.BeginTransaction();
        using (var command = connection.CreateCommand())
        {
            command.Transaction = transaction;
            command.CommandText = """
                ALTER TABLE communication_failure_journal RENAME TO communication_failure_journal_v5;

                CREATE TABLE communication_failure_journal (
                    failure_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
                    bus_id TEXT NOT NULL CHECK (length(trim(bus_id)) > 0),
                    modbus_address INTEGER NOT NULL CHECK (modbus_address BETWEEN 0 AND 255),
                    device_id INTEGER NULL CHECK (device_id IS NULL OR device_id BETWEEN 0 AND 4294967295),
                    operation TEXT NOT NULL CHECK (operation IN ('Polling', 'ExplicitRefresh', 'CommandTransaction', 'CampaignSelection')),
                    category TEXT NOT NULL CHECK (category IN ('Unclassified', 'Timeout', 'Io', 'ModbusExceptionResponse')),
                    observed_utc TEXT NOT NULL,
                    exception_type TEXT NOT NULL CHECK (length(trim(exception_type)) > 0),
                    message TEXT NOT NULL
                );

                INSERT INTO communication_failure_journal(
                    failure_id,
                    bus_id,
                    modbus_address,
                    device_id,
                    operation,
                    category,
                    observed_utc,
                    exception_type,
                    message)
                SELECT
                    failure_id,
                    bus_id,
                    modbus_address,
                    device_id,
                    operation,
                    'Unclassified',
                    observed_utc,
                    exception_type,
                    message
                FROM communication_failure_journal_v5;

                DROP TABLE communication_failure_journal_v5;

                CREATE INDEX ix_communication_failure_journal_endpoint_failure
                ON communication_failure_journal(bus_id, modbus_address, failure_id);
                CREATE INDEX ix_communication_failure_journal_device_failure
                ON communication_failure_journal(device_id, failure_id)
                WHERE device_id IS NOT NULL;
                CREATE INDEX ix_communication_failure_journal_category_failure
                ON communication_failure_journal(category, failure_id);
                """;
            command.ExecuteNonQuery();
        }
        SetUserVersion(connection, transaction, 6);
        transaction.Commit();
        return 6;
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
