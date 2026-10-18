namespace TR2.Application;

public interface ILiveArchiveReadSource
{
    // Newest rows first, bounded; reception time is not a window UTC.
    ValueTask<IReadOnlyList<LiveObservation>> ReadRecentAsync(CancellationToken ct = default);
}
public sealed record LiveSensorReadModel(LiveObservation Observation, LiveObservation? LastKnown,
    bool Historical, bool? TransportConnected, bool IsCurrent, double ReceptionAgeSeconds,
    string ArchiveState, string? ArchiveError);
public sealed record LiveOverviewResponse(DateTimeOffset ObservedAt, IReadOnlyList<LiveSensorReadModel> Devices,
    string Coverage, string? ArchiveError);

// No mutable cache or transport owner. Unknown endpoint identity is never promoted from an older row.
public sealed class LiveOverviewProjection(ILiveReadSource? current, ILiveArchiveReadSource? archive, bool physicalSession = false)
{
    public async ValueTask<LiveOverviewResponse> ReadAsync(DateTimeOffset now, CancellationToken ct = default)
    {
        IReadOnlyList<LiveObservation> stored = Array.Empty<LiveObservation>();
        string? error = null;
        try { if (archive is not null) stored = await archive.ReadRecentAsync(ct); }
        catch (OperationCanceledException) when (ct.IsCancellationRequested) { throw; }
        catch (Exception ex) { error = ex.Message; }
        var live = current?.Read(now) ?? Array.Empty<LiveReadView>();
        static string Endpoint(LiveObservation o) => o.BusId + "\0" + o.ModbusAddress;
        var endpoints = stored.Concat(live.Select(x => x.Observation)).GroupBy(Endpoint);
        var rows = new List<LiveSensorReadModel>();
        foreach (var endpoint in endpoints)
        {
            var session = live.FirstOrDefault(x => Endpoint(x.Observation) == endpoint.Key);
            var latest = session?.Observation ?? endpoint.OrderByDescending(x => x.ReceivedAt).First();
            // A changed durable identity cannot inherit measurements from the former sensor.
            // On identity loss, LastKnown explicitly keeps its own older owner and date.
            var known = endpoint.Where(x => x.Snapshot is { ValuesValid: true } &&
                (latest.DeviceId is null || x.DeviceId == latest.DeviceId))
                .OrderByDescending(x => x.ReceivedAt).FirstOrDefault();
            var historical = session is null || !physicalSession;
            rows.Add(new(latest, known, historical, historical ? null : session!.TransportConnected,
                !historical && session!.IsCurrent, Math.Max(0, (now - latest.ReceivedAt).TotalSeconds),
                session?.ArchiveState ?? "Persisted", session?.ArchiveError));
        }
        // Durable device identity, not COM, identifies a sensor. Unknown endpoints remain explicit.
        var devices = rows.GroupBy(x => x.Observation.DeviceId is {} id ? "device:" + id.Value : "endpoint:" + Endpoint(x.Observation))
            .Select(g => g.OrderByDescending(x => x.Observation.ReceivedAt).First())
            .OrderBy(x => x.Observation.DeviceId?.Value).ThenBy(x => x.Observation.BusId).ToArray();
        return new(now, devices, "Dernières 1000 observations SQLite ; couverture bornée, aucun inventaire exhaustif ni affectation équipement/local.", error);
    }
}
