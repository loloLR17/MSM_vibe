using TR2.Application;
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
            await error.WriteLineAsync("Usage: --live-archive <existing.db> --listen http://127.0.0.1:port ; copie historique figée sans writer/WAL, readonly, aucun port COM ouvert.");
            return 2;
        }
        var host = new SupervisionWebHost(new(uri, false), new SupervisionReadProjection(new FleetRegistry(),
            new DeviceTelemetrySnapshotRegistry(), new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10))),
            liveArchiveReadSource: new SqliteLiveArchiveReadSource(args[1], frozenSnapshot: true));
        try { await host.RunAsync(ct); return 0; }
        catch (OperationCanceledException) when (ct.IsCancellationRequested) { return 0; }
    }
}
