using System.Text.Json;
using Microsoft.Data.Sqlite;
using TR2.Application;

namespace TR2.Persistence.Sqlite;

// Read only: no database creation, migration, WAL policy or retention change.
public sealed class SqliteLiveArchiveReadSource(string databasePath, bool frozenSnapshot = false) : ILiveArchiveReadSource, ILiveHistoryReadSource
{
    public ValueTask<IReadOnlyList<LiveObservation>> ReadRecentAsync(CancellationToken ct = default)
    {
        ct.ThrowIfCancellationRequested();
        using var connection = OpenReadOnly();
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

    private SqliteConnection OpenReadOnly()
    {
        if (!File.Exists(databasePath)) throw new FileNotFoundException("Archive LIVE indisponible.", databasePath);
        if (frozenSnapshot && File.Exists(databasePath + "-wal") && new FileInfo(databasePath + "-wal").Length != 0)
            throw new InvalidDataException("Une archive figée doit être checkpointée et sans writer actif ; WAL non vide refusé.");
        var dataSource = frozenSnapshot ? new Uri(Path.GetFullPath(databasePath)).AbsoluteUri + "?immutable=1" : databasePath;
        var connection = new SqliteConnection(new SqliteConnectionStringBuilder
        { DataSource = dataSource, Mode = SqliteOpenMode.ReadOnly, Pooling = false }.ToString());
        try { connection.Open(); return connection; }
        catch { connection.Dispose(); throw; }
    }

    public ValueTask<LiveHistoryPage> ReadHistoryAsync(LiveHistoryQuery query, CancellationToken ct = default)
    {
        query.Validate(); ct.ThrowIfCancellationRequested();
        using var connection = OpenReadOnly();
        // PK seek bounds the scan independently of archive size and number of endpoints.
        using var bounds = connection.CreateCommand();
        bounds.CommandText = "SELECT row_id FROM live_observation WHERE row_id < $before ORDER BY row_id DESC LIMIT 10001";
        bounds.Parameters.AddWithValue("$before", query.Before ?? long.MaxValue);
        var ids = new List<long>();
        using (var reader = bounds.ExecuteReader())
            while (reader.Read()) { ct.ThrowIfCancellationRequested(); ids.Add(reader.GetInt64(0)); }
        var scan = Math.Min(10000, ids.Count);
        var entries = new List<LiveHistoryEntry>();
        if (scan == 0) return ValueTask.FromResult(new LiveHistoryPage(entries, null, 0, "Aucune ligne dans ce lot ; aucune rétention garantie."));
        using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT row_id, payload_json FROM live_observation
            WHERE row_id >= $oldest AND row_id <= $newest
              AND received_utc >= $from AND received_utc <= $to
              AND json_extract(payload_json, '$.BusId') = $bus
              AND json_extract(payload_json, '$.ModbusAddress') = $unit
            ORDER BY row_id DESC LIMIT $limit
            """;
        command.Parameters.AddWithValue("$oldest", ids[scan - 1]);
        command.Parameters.AddWithValue("$newest", ids[0]);
        command.Parameters.AddWithValue("$from", query.From.ToUniversalTime().ToString("O", System.Globalization.CultureInfo.InvariantCulture));
        command.Parameters.AddWithValue("$to", query.To.ToUniversalTime().ToString("O", System.Globalization.CultureInfo.InvariantCulture));
        command.Parameters.AddWithValue("$bus", query.Bus);
        command.Parameters.AddWithValue("$unit", query.Unit);
        command.Parameters.AddWithValue("$limit", query.Limit + 1);
        using (var reader = command.ExecuteReader())
            while (reader.Read())
            {
                ct.ThrowIfCancellationRequested();
                entries.Add(new(reader.GetInt64(0), JsonSerializer.Deserialize<LiveObservation>(reader.GetString(1))
                    ?? throw new InvalidDataException("Observation LIVE archivée nulle.")));
            }
        long? next = null;
        if (entries.Count > query.Limit) { entries.RemoveAt(entries.Count - 1); next = entries[^1].RowId; }
        else if (ids.Count > scan) next = ids[scan - 1];
        entries.Reverse(); // Actual insertion order: clock reversals are not silently sorted away.
        return ValueTask.FromResult(new LiveHistoryPage(entries, next, scan,
            "Réception PC ; ≤500 observations et ≤10000 lignes globales examinées par lot, période ≤7 jours. " +
            (next.HasValue ? "Couverture tronquée : lot antérieur disponible (éventuellement vide)." : "Fin de la couverture archivée pour cette période/endpoint ; rétention non garantie.")));
    }
}
