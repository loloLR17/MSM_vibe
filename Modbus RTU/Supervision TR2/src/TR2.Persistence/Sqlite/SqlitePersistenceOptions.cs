namespace TR2.Persistence.Sqlite;

public sealed class SqlitePersistenceOptions
{
    public SqlitePersistenceOptions(string databasePath, int busyTimeoutMilliseconds = 5000)
    {
        if (string.IsNullOrWhiteSpace(databasePath))
        {
            throw new ArgumentException("Database path must be provided.", nameof(databasePath));
        }

        if (busyTimeoutMilliseconds <= 0)
        {
            throw new ArgumentOutOfRangeException(nameof(busyTimeoutMilliseconds));
        }

        DatabasePath = Path.GetFullPath(databasePath);
        BusyTimeoutMilliseconds = busyTimeoutMilliseconds;
    }

    public string DatabasePath { get; }

    public int BusyTimeoutMilliseconds { get; }
}
