using System.Globalization;
using TR2.Application;

namespace TR2.Persistence.Sqlite;

public sealed class SqliteB3ArchiveSink : IB3ArchiveSink
{
    private readonly SqliteDatabase _database;

    public SqliteB3ArchiveSink(SqliteDatabase database)
    {
        _database = database ?? throw new ArgumentNullException(nameof(database));
    }

    public ValueTask AppendAsync(
        B3ArchiveObservation observation,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(observation);
        cancellationToken.ThrowIfCancellationRequested();

        using var connection = _database.OpenConnection();
        using var transaction = connection.BeginTransaction();
        using var command = connection.CreateCommand();
        command.Transaction = transaction;
        command.CommandText = """
            INSERT INTO b3_archive(
                device_id, received_utc,
                status_global, validity_flags, alarm_flags, severity_global,
                last_update_tr2_seconds, value_age_ms, calculation_sequence,
                window_duration_ms, valid_sample_count,
                rms_global_mg, peak_global_mg,
                rms_x_mg, rms_y_mg, rms_z_mg,
                peak_x_mg, peak_y_mg, peak_z_mg,
                dominant_axis, exceed_global, exceed_x, exceed_y, exceed_z,
                alarm_latched, exceed_count, alarm_count,
                b2_received_utc, b2_time_status, b2_time_flags,
                b2_current_time_seconds, b2_last_sync_time_seconds,
                b2_time_since_sync_seconds, b2_prepared_time_seconds,
                b2_prepared_time_status, b2_time_accuracy_ms,
                b2_drift_ppm, b2_sync_source)
            VALUES (
                $device_id, $received_utc,
                $status_global, $validity_flags, $alarm_flags, $severity_global,
                $last_update_tr2_seconds, $value_age_ms, $calculation_sequence,
                $window_duration_ms, $valid_sample_count,
                $rms_global_mg, $peak_global_mg,
                $rms_x_mg, $rms_y_mg, $rms_z_mg,
                $peak_x_mg, $peak_y_mg, $peak_z_mg,
                $dominant_axis, $exceed_global, $exceed_x, $exceed_y, $exceed_z,
                $alarm_latched, $exceed_count, $alarm_count,
                $b2_received_utc, $b2_time_status, $b2_time_flags,
                $b2_current_time_seconds, $b2_last_sync_time_seconds,
                $b2_time_since_sync_seconds, $b2_prepared_time_seconds,
                $b2_prepared_time_status, $b2_time_accuracy_ms,
                $b2_drift_ppm, $b2_sync_source);
            """;

        var value = observation.Value;
        command.Parameters.AddWithValue("$device_id", (long)observation.DeviceId.Value);
        command.Parameters.AddWithValue("$received_utc", UtcText(observation.ReceivedAt));
        command.Parameters.AddWithValue("$status_global", value.StatusGlobal);
        command.Parameters.AddWithValue("$validity_flags", value.ValidityFlags);
        command.Parameters.AddWithValue("$alarm_flags", value.AlarmFlags);
        command.Parameters.AddWithValue("$severity_global", value.SeverityGlobal);
        command.Parameters.AddWithValue("$last_update_tr2_seconds", (long)value.LastUpdateTr2Seconds);
        command.Parameters.AddWithValue("$value_age_ms", (long)value.ValueAgeMilliseconds);
        command.Parameters.AddWithValue("$calculation_sequence", (long)value.CalculationSequence);
        command.Parameters.AddWithValue("$window_duration_ms", (long)value.WindowDurationMilliseconds);
        command.Parameters.AddWithValue("$valid_sample_count", (long)value.ValidSampleCount);
        command.Parameters.AddWithValue("$rms_global_mg", (long)value.RmsGlobalMg);
        command.Parameters.AddWithValue("$peak_global_mg", (long)value.PeakGlobalMg);
        command.Parameters.AddWithValue("$rms_x_mg", (long)value.RmsXMg);
        command.Parameters.AddWithValue("$rms_y_mg", (long)value.RmsYMg);
        command.Parameters.AddWithValue("$rms_z_mg", (long)value.RmsZMg);
        command.Parameters.AddWithValue("$peak_x_mg", (long)value.PeakXMg);
        command.Parameters.AddWithValue("$peak_y_mg", (long)value.PeakYMg);
        command.Parameters.AddWithValue("$peak_z_mg", (long)value.PeakZMg);
        command.Parameters.AddWithValue("$dominant_axis", value.DominantAxis);
        command.Parameters.AddWithValue("$exceed_global", value.ExceedGlobal);
        command.Parameters.AddWithValue("$exceed_x", value.ExceedX);
        command.Parameters.AddWithValue("$exceed_y", value.ExceedY);
        command.Parameters.AddWithValue("$exceed_z", value.ExceedZ);
        command.Parameters.AddWithValue("$alarm_latched", value.AlarmLatched);
        command.Parameters.AddWithValue("$exceed_count", (long)value.ExceedCount);
        command.Parameters.AddWithValue("$alarm_count", (long)value.AlarmCount);

        var context = observation.TimeContext;
        command.Parameters.AddWithValue("$b2_received_utc", context is null ? DBNull.Value : UtcText(context.ReceivedAt));
        command.Parameters.AddWithValue("$b2_time_status", context is null ? DBNull.Value : context.Value.TimeStatus);
        command.Parameters.AddWithValue("$b2_time_flags", context is null ? DBNull.Value : context.Value.TimeFlags);
        command.Parameters.AddWithValue("$b2_current_time_seconds", context is null ? DBNull.Value : (long)context.Value.CurrentTimeSeconds);
        command.Parameters.AddWithValue("$b2_last_sync_time_seconds", context is null ? DBNull.Value : (long)context.Value.LastSyncTimeSeconds);
        command.Parameters.AddWithValue("$b2_time_since_sync_seconds", context is null ? DBNull.Value : (long)context.Value.TimeSinceSyncSeconds);
        command.Parameters.AddWithValue("$b2_prepared_time_seconds", context is null ? DBNull.Value : (long)context.Value.PreparedTimeSeconds);
        command.Parameters.AddWithValue("$b2_prepared_time_status", context is null ? DBNull.Value : context.Value.PreparedTimeStatus);
        command.Parameters.AddWithValue("$b2_time_accuracy_ms", context is null ? DBNull.Value : context.Value.TimeAccuracyMilliseconds);
        command.Parameters.AddWithValue("$b2_drift_ppm", context is null ? DBNull.Value : context.Value.DriftPpm);
        command.Parameters.AddWithValue("$b2_sync_source", context is null ? DBNull.Value : context.Value.SyncSource);

        command.ExecuteNonQuery();
        transaction.Commit();
        return ValueTask.CompletedTask;
    }

    private static string UtcText(DateTimeOffset value) =>
        value.ToUniversalTime().ToString("O", CultureInfo.InvariantCulture);
}
