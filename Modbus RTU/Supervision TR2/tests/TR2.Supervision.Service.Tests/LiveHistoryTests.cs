using System.IO.Compression;
using System.Net;
using System.Text.Json;
using Microsoft.Data.Sqlite;
using Microsoft.AspNetCore.Hosting.Server;
using Microsoft.AspNetCore.Hosting.Server.Features;
using Microsoft.Extensions.DependencyInjection;
using TR2.Application;
using TR2.Domain;
using TR2.Persistence.Sqlite;
using TR2.Supervision.Web;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class LiveHistoryTests
{
    private sealed class Archive : IDisposable
    {
        public string DirectoryPath { get; } = Path.Combine(Path.GetTempPath(), "uxl2-" + Guid.NewGuid());
        public string PathName => Path.Combine(DirectoryPath, "r5b2.db");
        public Archive()
        {
            Directory.CreateDirectory(DirectoryPath);
            var root = new DirectoryInfo(AppContext.BaseDirectory);
            while (root is not null && !File.Exists(Path.Combine(root.FullName, "AGENTS.md"))) root = root.Parent;
            using var input = File.OpenRead(Path.Combine(root!.FullName, "Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261018-R5B2/live.db.zlib"));
            using var z = new ZLibStream(input, CompressionMode.Decompress); using var output = File.Create(PathName); z.CopyTo(output);
        }
        public void Dispose() => Directory.Delete(DirectoryPath, true);
    }
    private static LiveHistoryQuery Query(int limit = 500, long? before = null) =>
        new("live-direct", 1, DateTimeOffset.Parse("2026-10-18T16:00:00Z"), DateTimeOffset.Parse("2026-10-18T18:00:00Z"), limit, before);

    [Fact]
    public async Task RealR5B2PagingPreservesAll528RowsQualityAndUnknownIdentityWithoutWrites()
    {
        using var a = new Archive(); var bytes = File.ReadAllBytes(a.PathName);
        var source = new SqliteLiveArchiveReadSource(a.PathName, true);
        var entries = new List<LiveHistoryEntry>(); long? cursor = null;
        do
        {
            var page = await source.ReadHistoryAsync(Query(37, cursor));
            Assert.InRange(page.Entries.Count, 0, 37); Assert.InRange(page.ScannedRows, 0, 10000);
            Assert.True(page.Historical); Assert.False(page.IsCurrent);
            Assert.Equal(page.Entries.OrderBy(e => e.RowId).ToArray(), page.Entries);
            entries.AddRange(page.Entries); cursor = page.NextBefore;
        } while (cursor is not null);
        Assert.Equal(528, entries.Count); Assert.Equal(528, entries.Select(e => e.RowId).Distinct().Count());
        Assert.Contains(entries, e => e.Observation.Snapshot is { ValuesValid: true, Fresh: true });
        Assert.Contains(entries, e => e.Observation.Snapshot is { Held: true });
        Assert.Contains(entries, e => e.Observation.Snapshot is null && e.Observation.DeviceId is null);
        Assert.All(entries, e => { Assert.Null(e.Observation.Snapshot?.WindowUtc); Assert.Null(e.Observation.RunId); Assert.Null(e.Observation.ConfigurationId); });
        Assert.Equal(bytes, File.ReadAllBytes(a.PathName)); Assert.False(File.Exists(a.PathName + "-wal"));
    }

    [Fact]
    public async Task BoundsPeriodAndParametersCannotEscapeEndpointOrCreateMissingDatabase()
    {
        using var a = new Archive(); var source = new SqliteLiveArchiveReadSource(a.PathName, true);
        foreach (var q in new[] { Query() with { Bus = "' OR 1=1 --" }, Query() with { Unit = 247 }, Query() with { From = DateTimeOffset.Parse("2026-10-17T16:00Z"), To = DateTimeOffset.Parse("2026-10-17T18:00Z") } })
        { var page = await source.ReadHistoryAsync(q); Assert.Empty(page.Entries); Assert.Null(page.NextBefore); }
        var missing = Path.Combine(a.DirectoryPath, "missing.db");
        await Assert.ThrowsAsync<FileNotFoundException>(async () => await new SqliteLiveArchiveReadSource(missing).ReadHistoryAsync(Query()));
        Assert.False(File.Exists(missing));
        File.WriteAllBytes(a.PathName + "-wal", [1, 2]);
        await Assert.ThrowsAsync<InvalidDataException>(async () => await source.ReadHistoryAsync(Query()));
        Assert.Equal(new byte[] { 1, 2 }, File.ReadAllBytes(a.PathName + "-wal"));
    }

    [Theory]
    [InlineData(0, 500, 1)] [InlineData(248, 500, 1)] [InlineData(1, 0, 1)]
    [InlineData(1, 501, 1)] [InlineData(1, 500, 0)] [InlineData(1, 500, -1)]
    public void InvalidBoundsAreRejected(byte unit, int limit, long before) =>
        Assert.Throws<ArgumentException>(() => (Query(limit, before) with { Unit = unit }).Validate());

    [Fact]
    public void EmptyBusReversedOrTooLongPeriodAreRejected()
    {
        foreach (var q in new[] { Query() with { Bus = "" }, Query() with { Bus = new string('x',129) }, Query() with { To = Query().From }, Query() with { To = Query().From.AddDays(8) } })
            Assert.Throws<ArgumentException>(q.Validate);
    }

    [Fact]
    public async Task SyntheticFiftyEndpointArchiveBoundsScanAndAllowsEmptyOlderPages()
    {
        using var a = new Archive();
        // Synthetic load fixture ONLY, derived from a captured JSON shape; no acquisition claim.
        using (var db = new SqliteConnection("Data Source=" + a.PathName))
        {
            db.Open(); using var cmd = db.CreateCommand();
            cmd.CommandText = """
                CREATE TABLE original_shape AS SELECT * FROM live_observation LIMIT 1;
                DELETE FROM live_observation;
                WITH RECURSIVE n(x) AS (SELECT 1 UNION ALL SELECT x+1 FROM n WHERE x<50000)
                INSERT INTO live_observation(observation_id,device_id,segment_id,received_utc,protocol_version,profile,continuity,values_valid,fresh,payload_json)
                SELECT 'synthetic-'||x,device_id,segment_id,received_utc,protocol_version,profile,continuity,values_valid,fresh,
                    json_set(payload_json,'$.BusId','synthetic-'||(x%50),'$.Source','SYNTHETIC SOFTWARE LOAD FIXTURE')
                FROM n CROSS JOIN original_shape;
                """;
            cmd.ExecuteNonQuery();
        }
        var source = new SqliteLiveArchiveReadSource(a.PathName, true); var timer = System.Diagnostics.Stopwatch.StartNew();
        var page = await source.ReadHistoryAsync(Query() with { Bus = "synthetic-1" });
        Assert.Equal(10000, page.ScannedRows); Assert.Equal(200, page.Entries.Count); Assert.NotNull(page.NextBefore);
        Assert.Contains("tronquée", page.Coverage);
        var empty = await source.ReadHistoryAsync(Query() with { Bus = "absent" });
        Assert.Empty(empty.Entries); Assert.NotNull(empty.NextBefore); Assert.Equal(10000, empty.ScannedRows);
        var older = await source.ReadHistoryAsync(Query(before: page.NextBefore) with { Bus = "synthetic-1" });
        Assert.Equal(200, older.Entries.Count); Assert.Empty(page.Entries.Select(e=>e.RowId).Intersect(older.Entries.Select(e=>e.RowId)));
        Console.WriteLine($"UXL2 synthetic 50000 rows / 50 endpoints: three bounded reads {timer.Elapsed.TotalMilliseconds:F1} ms (software environment only)");
    }

    [Fact]
    public async Task HttpHistoryIsAdditiveBoundedReadonlyAndReportsArchiveErrors()
    {
        using var a = new Archive();
        var projection = new SupervisionReadProjection(new FleetRegistry(), new DeviceTelemetrySnapshotRegistry(), new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10)));
        var host = new SupervisionWebHost(new(new Uri("http://127.0.0.1:0"), false), projection, liveArchiveReadSource: new SqliteLiveArchiveReadSource(a.PathName, true));
        await using var app = host.CreateApplication(); await app.StartAsync();
        try
        {
            using var client = new HttpClient { BaseAddress = new Uri(app.Services.GetRequiredService<IServer>().Features.Get<IServerAddressesFeature>()!.Addresses.Single()) };
            const string q = "/api/v2/live/history?bus=live-direct&unit=1&from=2026-10-18T16:00:00Z&to=2026-10-18T18:00:00Z";
            using var json = JsonDocument.Parse(await client.GetStringAsync(q + "&limit=23"));
            Assert.Equal(23, json.RootElement.GetProperty("entries").GetArrayLength());
            Assert.True(json.RootElement.GetProperty("historical").GetBoolean());
            Assert.Equal(HttpStatusCode.BadRequest, (await client.GetAsync(q + "&limit=501")).StatusCode);
            Assert.Equal(HttpStatusCode.BadRequest, (await client.GetAsync(q + "&before=-1")).StatusCode);
            Assert.Equal(HttpStatusCode.BadRequest, (await client.GetAsync(q.Replace("unit=1", "unit=x", StringComparison.Ordinal))).StatusCode);
            Assert.True((await client.GetAsync("/api/v1/fleet")).IsSuccessStatusCode);
            Assert.True((await client.GetAsync("/live-trends.js")).IsSuccessStatusCode);
            File.WriteAllBytes(a.PathName + "-wal", [1]);
            Assert.Equal(HttpStatusCode.ServiceUnavailable, (await client.GetAsync(q)).StatusCode);
        }
        finally { await app.StopAsync(); }
    }
}
