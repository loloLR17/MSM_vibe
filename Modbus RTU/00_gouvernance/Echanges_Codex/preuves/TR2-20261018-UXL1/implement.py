from pathlib import Path
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-UXL1-live-readonly');s=r/'Modbus RTU/Supervision TR2/src'
(s/'TR2.Application/LiveOverviewProjection.cs').write_text('''namespace TR2.Application;

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
        static string Endpoint(LiveObservation o) => o.BusId + "\\0" + o.ModbusAddress;
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
''')
(s/'TR2.Persistence/Sqlite/SqliteLiveArchiveReadSource.cs').write_text('''using System.Text.Json;
using Microsoft.Data.Sqlite;
using TR2.Application;

namespace TR2.Persistence.Sqlite;

// Read only: no database creation, migration, WAL policy or retention change.
public sealed class SqliteLiveArchiveReadSource(string databasePath) : ILiveArchiveReadSource
{
    public ValueTask<IReadOnlyList<LiveObservation>> ReadRecentAsync(CancellationToken ct = default)
    {
        ct.ThrowIfCancellationRequested();
        if (!File.Exists(databasePath)) throw new FileNotFoundException("Archive LIVE indisponible.", databasePath);
        using var connection = new SqliteConnection(new SqliteConnectionStringBuilder
        { DataSource = databasePath, Mode = SqliteOpenMode.ReadOnly, Pooling = false }.ToString());
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
''')
p=s/'TR2.Supervision.Web/SupervisionWebHost.cs';t=p.read_text().replace('    private const int MaxRequestIdentityLength', '    private readonly LiveOverviewProjection _liveOverview;\n    private const int MaxRequestIdentityLength').replace('ILiveReadSource? liveReadSource = null)','ILiveReadSource? liveReadSource = null,\n        ILiveArchiveReadSource? liveArchiveReadSource = null, bool liveDataIsPhysical = false)').replace('        _liveReadSource = liveReadSource;', '        _liveReadSource = liveReadSource;\n        _liveOverview = new(liveReadSource, liveArchiveReadSource, liveDataIsPhysical);').replace('        application.MapGet("/api/v2/live",', '        application.MapGet("/api/v2/live/overview", async (CancellationToken ct) =>\n            Results.Ok(await _liveOverview.ReadAsync(_timeProvider.GetUtcNow(), ct)));\n\n        application.MapGet("/api/v2/live",');p.write_text(t)
for name,db,physical in [('LiveDirectConsoleApplication.cs','args[4]','true'),('LiveReplayConsoleApplication.cs','args[3]','false')]:
 p=s/'TR2.Supervision.Service'/name;t=p.read_text().replace('liveReadSource: source);',f'liveReadSource: source, liveArchiveReadSource: new SqliteLiveArchiveReadSource({db}), liveDataIsPhysical: {physical});');p.write_text(t)
(s/'TR2.Supervision.Service/LiveArchiveConsoleApplication.cs').write_text('''using TR2.Application;
using TR2.Persistence.Sqlite;
using TR2.Supervision.Web;

namespace TR2.Supervision.Service;

public static class LiveArchiveConsoleApplication
{
    // Existing service host, durable archive only. No serial factory, writer or replay polling.
    public static async Task<int> RunAsync(IReadOnlyList<string> args, CancellationToken ct, TextWriter error)
    {
        if (args.Count != 4 || args[2] != "--listen" || !File.Exists(args[1]) ||
            !Uri.TryCreate(args[3], UriKind.Absolute, out var uri) || uri.Scheme != "http" || !uri.IsLoopback)
        {
            await error.WriteLineAsync("Usage: --live-archive <existing.db> --listen http://127.0.0.1:port ; historique readonly, aucun port COM ouvert.");
            return 2;
        }
        var host = new SupervisionWebHost(new(uri, false), new SupervisionReadProjection(new FleetRegistry(),
            new DeviceTelemetrySnapshotRegistry(), new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10))),
            liveArchiveReadSource: new SqliteLiveArchiveReadSource(args[1]));
        try { await host.RunAsync(ct); return 0; }
        catch (OperationCanceledException) when (ct.IsCancellationRequested) { return 0; }
    }
}
''')
p=s/'TR2.Supervision.Service/SupervisionConsoleApplication.cs';t=p.read_text().replace('        if (args.Count > 0 && args[0] == "--live-direct")','        if (args.Count > 0 && args[0] == "--live-archive")\n            return await LiveArchiveConsoleApplication.RunAsync(args, cancellationToken, error);\n\n        if (args.Count > 0 && args[0] == "--live-direct")');p.write_text(t)
web=s/'TR2.Supervision.Web/wwwroot'
# Embed LIVE overview inside the current general view; legacy V1 source and controls remain separate.
p=web/'index.html';t=p.read_text().replace('<h1>Vue générale</h1>','<h1>Vue générale · profil V1</h1>').replace('        <a class="nav-link" href="/live.html">LIVE · lecture seule</a>', '        <a class="nav-link" href="/live-fleet.html">Parc LIVE</a>\n        <a class="nav-link" href="/live.html">Diagnostic LIVE</a>');t=t.replace('      <section>', '''      <section aria-label="Parc LIVE">
        <div class="page-head"><h2>Parc LIVE · lecture seule</h2><a href="/live-fleet.html">Ouvrir le parc LIVE</a></div>
        <p id="liveOverviewState" role="status"></p>
        <div class="table-card"><table><thead><tr><th>Capteur / endpoint</th><th>RMS global</th><th>Crête globale</th><th>Sévérité B3</th><th>Communication / qualité</th><th>Réception PC</th></tr></thead><tbody id="liveFleetRows"></tbody></table></div>
      </section>
      <section>''',1).replace('  <script src="/app.js"></script>', '  <script src="/app.js"></script>\n  <script src="/live-overview.js"></script>');p.write_text(t)
nav='''<aside class="sidebar"><div class="brand">MSM<span>Supervision vibrations</span></div><nav><a class="nav-link" href="/">Vue générale</a><a class="nav-link" href="/live-fleet.html">Parc LIVE</a><a class="nav-link" href="/live.html">Diagnostic LIVE</a></nav><p class="scope">Lecture seule · profil LIVE distinct du V1</p></aside>'''
head='''<!doctype html><html lang="fr"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>MSM — LIVE</title><link rel="stylesheet" href="/styles.css"></head><body><div class="app-shell">'''
end='''</main></div><script src="/live-overview.js"></script></body></html>'''
(web/'live-fleet.html').write_text(head+nav+'''<main><div class="page-head"><h1>Vue générale LIVE</h1></div><p id="liveOverviewState" role="status"></p><div class="table-card"><table><thead><tr><th>Capteur / endpoint</th><th>RMS global</th><th>Crête globale</th><th>Sévérité B3</th><th>Communication / qualité</th><th>Réception PC</th></tr></thead><tbody id="liveFleetRows"></tbody></table></div><p class="scope-note">Pas d'affectation équipement/local disponible. Filtres avancés, tendance et export reportés à UX-L2. Aucune valeur absente remplacée par zéro.</p>'''+end)
tabs=''.join(f'<button class="nav-item" data-live-tab="{key}">{name}</button>' for key,name in [('summary','Synthèse'),('vibration','Vibrations'),('state','État & configuration'),('commands','Commandes'),('campaigns','Campagnes'),('diagnostic','Diagnostic')]);(web/'live-device.html').write_text(head+nav.replace('</nav>',tabs+'</nav>')+'''<main><a class="back-link" href="/live-fleet.html">← Parc LIVE</a><h1 id="liveDeviceTitle">Fiche LIVE</h1><p id="liveOverviewState" role="status"></p><div id="liveDeviceContent" class="detail-card"></div><p class="scope-note">LiveDiagnostic : NON-PRODUCTION, profil partiel ; B1/B2/B7 indisponibles, UTC de fenêtre et run/epoch/config non exposés. Une archive n'est pas une acquisition actuelle.</p>'''+end)
