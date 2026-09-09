using System.Globalization;
using Microsoft.Data.Sqlite;

namespace TR2.Persistence.Sqlite;

public sealed class SqliteDatabase
{
    public const int CurrentSchemaVersion = 1;

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
        {
            throw new InvalidOperationException("SQLite synchronous=FULL could not be verified.");
        }

        if (ReadInt32(connection, "PRAGMA foreign_keys;") != 1)
        {
            throw new InvalidOperationException("SQLite foreign_keys=ON could not be verified.");
        }

        if (ReadInt32(connection, "PRAGMA busy_timeout;") != _options.BusyTimeoutMilliseconds)
        {
            throw new InvalidOperationException("SQLite busy_timeout could not be verified.");
        }
    }

    private static void ApplyMigrations(SqliteConnection connection)
    {
        var version = ReadInt32(connection, "PRAGMA user_version;");

        if (version > CurrentSchemaVersion)
        {
            throw new NotSupportedException(
                $"SQLite schema version {version} is newer than supported version {CurrentSchemaVersion}.");
        }

        while (version < CurrentSchemaVersion)
        {
            version = version switch
            {
                0 => ApplyMigration1(connection),
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

        using (var command = connection.CreateCommand())
        {
            command.Transaction = transaction;
            command.CommandText = "PRAGMA user_version = 1;";
            command.ExecuteNonQuery();
        }

        transaction.Commit();
        return 1;
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
