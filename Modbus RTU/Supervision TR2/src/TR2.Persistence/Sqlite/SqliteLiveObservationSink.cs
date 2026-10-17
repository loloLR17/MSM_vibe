using System.Globalization;
using System.Text.Json;
using TR2.Application;

namespace TR2.Persistence.Sqlite;

public sealed class SqliteLiveObservationSink(SqliteDatabase database) : ILiveObservationSink
{
    public ValueTask AppendAsync(LiveObservation observation, CancellationToken ct = default)
    {
        ct.ThrowIfCancellationRequested();
        using var connection = database.OpenConnection();
        using var command = connection.CreateCommand();
        command.CommandText = """
            INSERT INTO live_observation(observation_id,device_id,segment_id,received_utc,
                protocol_version,profile,continuity,values_valid,fresh,payload_json)
            VALUES($id,$device,$segment,$received,$protocol,$profile,$continuity,$valid,$fresh,$payload);
            """;
        command.Parameters.AddWithValue("$id", observation.ObservationId.ToString());
        command.Parameters.AddWithValue("$device", observation.DeviceId is {} id ? (long)id.Value : DBNull.Value);
        command.Parameters.AddWithValue("$segment", observation.SegmentId.ToString());
        command.Parameters.AddWithValue("$received", observation.ReceivedAt.ToUniversalTime().ToString("O", CultureInfo.InvariantCulture));
        command.Parameters.AddWithValue("$protocol", observation.Identity is {} identity ? identity.ProtocolVersion : DBNull.Value);
        command.Parameters.AddWithValue("$profile", observation.Identity?.Profile.ToString() is {} profile ? profile : DBNull.Value);
        command.Parameters.AddWithValue("$continuity", observation.Continuity.ToString());
        command.Parameters.AddWithValue("$valid", observation.Snapshot?.ValuesValid == true ? 1 : 0);
        command.Parameters.AddWithValue("$fresh", observation.Snapshot?.Fresh == true ? 1 : 0);
        command.Parameters.AddWithValue("$payload", JsonSerializer.Serialize(observation));
        command.ExecuteNonQuery();
        return ValueTask.CompletedTask;
    }
}
