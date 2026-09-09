using Microsoft.Data.Sqlite;
using TR2.Persistence.Sqlite;
using Xunit;

namespace TR2.Persistence.Tests;

public sealed class SqliteDatabaseTests : IDisposable
{
    private readonly string _directory = Path.Combine(Path.GetTempPath(), $"tr2-persistence-{Guid.NewGuid():N}");

    [Fact]
    public void OpenConnection_CreatesDatabaseWithRequiredPragmasAndSchemaVersion()
    {
        var databasePath = Path.Combine(_directory, "supervision.db");
        var database = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));

        using var connection = database.OpenConnection();

        Assert.True(File.Exists(databasePath));
        Assert.Equal("wal", ReadString(connection, "PRAGMA journal_mode;"), ignoreCase: true);
        Assert.Equal(2, ReadInt32(connection, "PRAGMA synchronous;"));
        Assert.Equal(1, ReadInt32(connection, "PRAGMA foreign_keys;"));
        Assert.Equal(5000, ReadInt32(connection, "PRAGMA busy_timeout;"));
        Assert.Equal(SqliteDatabase.CurrentSchemaVersion, ReadInt32(connection, "PRAGMA user_version;"));
        Assert.Equal(1, ReadInt32(connection, "SELECT COUNT(*) FROM tr2_schema_marker WHERE singleton = 1;"));
    }

    [Fact]
    public void OpenConnection_ReopensExistingDatabaseWithoutReapplyingMigration()
    {
        var databasePath = Path.Combine(_directory, "reopen.db");
        var database = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));

        using (var firstConnection = database.OpenConnection())
        {
            Assert.Equal(1, ReadInt32(firstConnection, "SELECT COUNT(*) FROM tr2_schema_marker;"));
        }

        using var secondConnection = database.OpenConnection();

        Assert.Equal(SqliteDatabase.CurrentSchemaVersion, ReadInt32(secondConnection, "PRAGMA user_version;"));
        Assert.Equal(1, ReadInt32(secondConnection, "SELECT COUNT(*) FROM tr2_schema_marker;"));
    }

    [Fact]
    public void OpenConnection_RejectsDatabaseNewerThanSupportedSchema()
    {
        var databasePath = Path.Combine(_directory, "future.db");
        var database = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));

        using (var connection = database.OpenConnection())
        {
            using var command = connection.CreateCommand();
            command.CommandText = $"PRAGMA user_version = {SqliteDatabase.CurrentSchemaVersion + 1};";
            command.ExecuteNonQuery();
        }

        var exception = Assert.Throws<NotSupportedException>(() => database.OpenConnection());
        Assert.Contains("newer than supported", exception.Message, StringComparison.OrdinalIgnoreCase);
    }

    [Fact]
    public void OpenConnection_UsesConfiguredBusyTimeout()
    {
        var databasePath = Path.Combine(_directory, "timeout.db");
        var database = new SqliteDatabase(new SqlitePersistenceOptions(databasePath, busyTimeoutMilliseconds: 2500));

        using var connection = database.OpenConnection();

        Assert.Equal(2500, ReadInt32(connection, "PRAGMA busy_timeout;"));
    }

    public void Dispose()
    {
        SqliteConnection.ClearAllPools();

        if (Directory.Exists(_directory))
        {
            Directory.Delete(_directory, recursive: true);
        }
    }

    private static int ReadInt32(SqliteConnection connection, string sql)
    {
        using var command = connection.CreateCommand();
        command.CommandText = sql;
        return Convert.ToInt32(command.ExecuteScalar(), System.Globalization.CultureInfo.InvariantCulture);
    }

    private static string ReadString(SqliteConnection connection, string sql)
    {
        using var command = connection.CreateCommand();
        command.CommandText = sql;
        return Convert.ToString(command.ExecuteScalar(), System.Globalization.CultureInfo.InvariantCulture) ?? string.Empty;
    }
}
