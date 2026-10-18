using System.Text.Json;
using Microsoft.Data.Sqlite;
using TR2.Application;

namespace TR2.Persistence.Sqlite;

// Read only: no database creation, migration, WAL policy or retention change.
public sealed class SqliteLiveArchiveReadSource(string databasePath, bool frozenSnapshot = false) : ILiveArchiveReadSource
{
    public ValueTask<IReadOnlyList<LiveObservation>> ReadRecentAsync(CancellationToken ct = default)
    {
        ct.ThrowIfCancellationRequested();
        if (!File.Exists(databasePath)) throw new FileNotFoundException("Archive LIVE indisponible.", databasePath);
        if (frozenSnapshot && File.Exists(databasePath + "-wal") && new FileInfo(databasePath + "-wal").Length != 0)
            throw new InvalidDataException("Une archive figée doit être checkpointée et sans writer actif ; WAL non vide refusé.");
        var dataSource = frozenSnapshot ? new Uri(Path.GetFullPath(databasePath)).AbsoluteUri + "?immutable=1" : databasePath;
        using var connection = new SqliteConnection(new SqliteConnectionStringBuilder
        { DataSource = dataSource, Mode = SqliteOpenMode.ReadOnly, Pooling = false }.ToString());
        connection.Open();
        using var command = connection.CreateCommand();
        command.CommandText = "SELECT payload_json FROM live_observation ORDER BY row_id DESC LIMIT 1000";
        using var reader = command.ExecuteReader();
        var result = new List<LiveObservation>();
        while (reader.Read())
        {
            ct.ThrowIfCancellationRequested();
            result.Add(JsonSerializer.Deserialize<LiveObservation>(reader.GetString(0))
                ?? throw new InvalidDataException("Observation LIVE archivée nulle."));
        }
        return ValueTask.FromResult<IReadOnlyList<LiveObservation>>(result);
    }
}
