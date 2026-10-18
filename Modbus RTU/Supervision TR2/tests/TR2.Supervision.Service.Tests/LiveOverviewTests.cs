using System.IO.Compression;
using System.Text.Json;
using Microsoft.Data.Sqlite;
using TR2.Application;
using TR2.Domain;
using TR2.Persistence.Sqlite;
using Xunit;
using TR2.Supervision.Web;
using Microsoft.AspNetCore.Hosting.Server;
using Microsoft.AspNetCore.Hosting.Server.Features;
using Microsoft.Extensions.DependencyInjection;

namespace TR2.Supervision.Service.Tests;

public sealed class LiveOverviewTests
{
    private static string Root()
    {
        var d = new DirectoryInfo(AppContext.BaseDirectory);
        while (d is not null && !File.Exists(Path.Combine(d.FullName, "AGENTS.md"))) d = d.Parent;
        return d!.FullName;
    }
    private static async Task<List<LiveObservation>> PhysicalViews()
    {
        var path = Path.Combine(Root(), "Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261018-R5B2/running-service.log.zlib");
        using var file = File.OpenRead(path); using var z = new ZLibStream(file, CompressionMode.Decompress);
        using var reader = new StreamReader(z); var rows = new List<LiveObservation>();
        foreach (var line in (await reader.ReadToEndAsync()).Split('\n'))
        {
            if (!line.StartsWith("{\"Observation\"", StringComparison.Ordinal)) continue;
            using var doc = JsonDocument.Parse(line);
            rows.Add(JsonSerializer.Deserialize<LiveObservation>(doc.RootElement.GetProperty("Observation"))!);
        }
        Assert.Contains(rows, x => x.Snapshot is { Fresh: true, ValuesValid: true });
        Assert.Contains(rows, x => x.Snapshot is null);
        return rows;
    }

    [Fact]
    public async Task AuthenticR5B2SqliteArchiveIsReadonlyAndNeverCurrentAfterRestart()
    {
        var dir = Path.Combine(Path.GetTempPath(), "uxl1-" + Guid.NewGuid()); Directory.CreateDirectory(dir);
        var path = Path.Combine(dir, "history.db");
        try
        {
            var source = Path.Combine(Root(), "Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261018-R5B2/live.db.zlib");
            using (var input = File.OpenRead(source)) using (var z = new ZLibStream(input, CompressionMode.Decompress))
            using (var output = File.Create(path)) await z.CopyToAsync(output);
            var before = await File.ReadAllBytesAsync(path);
            var archive = new SqliteLiveArchiveReadSource(path, frozenSnapshot: true);
            var rows = await archive.ReadRecentAsync(); Assert.Equal(528, rows.Count);
            var overview = await new LiveOverviewProjection(null, archive).ReadAsync(DateTimeOffset.UtcNow);
            var identified = Assert.Single(overview.Devices, x => x.Observation.DeviceId is not null);
            Assert.Equal(1u, identified.Observation.DeviceId!.Value.Value);
            Assert.True(identified.Historical); Assert.False(identified.IsCurrent); Assert.Null(identified.TransportConnected);
            Assert.True(identified.Observation.Snapshot!.Held); Assert.NotNull(identified.LastKnown);
            Assert.Null(identified.Observation.Snapshot.WindowUtc);
            Assert.Equal(before, await File.ReadAllBytesAsync(path));
            Assert.False(File.Exists(path + "-wal"));
        }
        finally { Directory.Delete(dir, true); }
    }

    [Fact]
    public async Task PhysicalLossRetainsExplicitOlderOwnerButNeverClaimsCurrentIdentityOrFreshness()
    {
        var rows = await PhysicalViews(); var good = rows.First(x => x.Snapshot is { Fresh: true });
        var lost = rows.First(x => x.Snapshot is null);
        var live = new LiveSource(new(lost, false, false, false, "Persisted", null));
        var result = await new LiveOverviewProjection(live, new Archive(good), true).ReadAsync(lost.ReceivedAt);
        var row = Assert.Single(result.Devices); Assert.Null(row.Observation.DeviceId);
        Assert.Equal(good.ObservationId, row.LastKnown!.ObservationId);
        Assert.False(row.IsCurrent); Assert.False(row.TransportConnected); Assert.False(row.Historical);
    }

    [Fact]
    public async Task ReplayAndChangedIdentityCannotPromoteOldMeasurements()
    {
        var good = (await PhysicalViews()).First(x => x.Snapshot is { Fresh: true });
        var view = new LiveReadView(good, true, false, true, "Persisted", null);
        var replay = await new LiveOverviewProjection(new LiveSource(view), new Archive(good)).ReadAsync(good.ReceivedAt);
        Assert.False(Assert.Single(replay.Devices).IsCurrent); Assert.True(replay.Devices[0].Historical);
        var changed = good with { DeviceId = new DeviceId(2), Snapshot = null, ReceivedAt = good.ReceivedAt.AddSeconds(1) };
        var different = await new LiveOverviewProjection(new LiveSource(view with { Observation = changed, IsCurrent = false }), new Archive(good), true).ReadAsync(changed.ReceivedAt);
        Assert.Null(Assert.Single(different.Devices).LastKnown);
    }

    [Fact]
    public async Task DurableIdentityDeduplicatesEndpointsAndArchiveFailureIsVisible()
    {
        var good = (await PhysicalViews()).First(x => x.Snapshot is { Fresh: true });
        var moved = good with { BusId = "second endpoint", ReceivedAt = good.ReceivedAt.AddSeconds(1) };
        var projected = await new LiveOverviewProjection(null, new Archive(good, moved)).ReadAsync(moved.ReceivedAt);
        Assert.Equal("second endpoint", Assert.Single(projected.Devices).Observation.BusId);
        var bad = await new LiveOverviewProjection(null, new SqliteLiveArchiveReadSource("/no-such-uxl1-archive.db")).ReadAsync(DateTimeOffset.UtcNow);
        Assert.NotNull(bad.ArchiveError); Assert.Empty(bad.Devices);
    }

    [Fact]
    public async Task FrozenSnapshotRejectsNonemptyWalWithoutOpeningOrChangingIt()
    {
        var dir = Path.Combine(Path.GetTempPath(), "uxl1-wal-" + Guid.NewGuid()); Directory.CreateDirectory(dir);
        var file = Path.Combine(dir, "archive.db");
        try
        {
            await File.WriteAllBytesAsync(file, [1, 2, 3]);
            await File.WriteAllBytesAsync(file + "-wal", [4, 5]);
            await Assert.ThrowsAsync<InvalidDataException>(async () => await new SqliteLiveArchiveReadSource(file, frozenSnapshot: true).ReadRecentAsync());
            Assert.Equal(new byte[] { 1, 2, 3 }, await File.ReadAllBytesAsync(file));
            Assert.Equal(new byte[] { 4, 5 }, await File.ReadAllBytesAsync(file + "-wal"));
        }
        finally { Directory.Delete(dir, true); }
    }

    [Fact]
    public async Task AdditiveHttpApiAndSixTabsKeepV1AndDiagnosticAvailableWithoutCommands()
    {
        var good = (await PhysicalViews()).First(x => x.Snapshot is { Fresh: true });
        var projection = new SupervisionReadProjection(new FleetRegistry(), new DeviceTelemetrySnapshotRegistry(),
            new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10)));
        var host = new SupervisionWebHost(new(new Uri("http://127.0.0.1:0"), false), projection,
            liveArchiveReadSource: new Archive(good));
        await using var app = host.CreateApplication(); await app.StartAsync();
        try
        {
            var address = app.Services.GetRequiredService<IServer>().Features.Get<IServerAddressesFeature>()!.Addresses.Single();
            using var client = new HttpClient { BaseAddress = new Uri(address) };
            foreach (var path in new[] { "/", "/live-fleet.html", "/live-device.html", "/live.html", "/live-overview.js", "/api/v1/fleet" })
                Assert.True((await client.GetAsync(path)).IsSuccessStatusCode, path);
            using var api = JsonDocument.Parse(await client.GetStringAsync("/api/v2/live/overview"));
            Assert.True(api.RootElement.GetProperty("devices")[0].GetProperty("historical").GetBoolean());
            Assert.False(api.RootElement.GetProperty("devices")[0].GetProperty("isCurrent").GetBoolean());
            Assert.Equal(404, (int)(await client.PostAsync("/api/v1/devices/1/commands", new StringContent("{}", System.Text.Encoding.UTF8, "application/json"))).StatusCode);
            var html = await client.GetStringAsync("/live-device.html");
            Assert.Equal(6, System.Text.RegularExpressions.Regex.Matches(html, "data-live-tab=").Count);
            Assert.DoesNotContain("/commands.js", html);
        }
        finally { await app.StopAsync(); }
    }

    private sealed class LiveSource(LiveReadView view) : ILiveReadSource
    { public IReadOnlyList<LiveReadView> Read(DateTimeOffset now) => [view]; }
    private sealed class Archive(params LiveObservation[] rows) : ILiveArchiveReadSource
    { public ValueTask<IReadOnlyList<LiveObservation>> ReadRecentAsync(CancellationToken ct = default) => ValueTask.FromResult<IReadOnlyList<LiveObservation>>(rows); }
}
