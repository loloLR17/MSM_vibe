using System.IO.Compression;
using System.Text;
using System.Text.Json;
using Microsoft.AspNetCore.Hosting.Server;
using Microsoft.AspNetCore.Hosting.Server.Features;
using Microsoft.Data.Sqlite;
using Microsoft.Extensions.DependencyInjection;
using TR2.Application;
using TR2.Domain;
using TR2.Persistence.Sqlite;
using TR2.Protocol;
using TR2.Supervision.Web;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class LiveSupervisionTests : IDisposable
{
    private readonly string _directory = Path.Combine(Path.GetTempPath(), "tr2-live-" + Guid.NewGuid());
    private static readonly TR2Endpoint Endpoint = new(new SerialBus("test"), new ModbusAddress(1));
    private static readonly DateTimeOffset Now = new(2026, 10, 17, 12, 0, 0, TimeSpan.Zero);

    [Theory]
    [InlineData("TR2-20261017-R4U", "hardware/live05-master.jsonl.zlib", 1632)]
    [InlineData("TR2-20261017-R4V", "hardware/baseline-master.jsonl.zlib", 1655)]
    [InlineData("TR2-20261017-R4V", "hardware/paced-master.jsonl.zlib", 1075)]
    public async Task AuthenticPhysicalAdusAreQuarantinedForProvenFirmwareOffsetViolation(string mission, string name, int expected)
    {
        var root = FindRepository();
        var path = Path.Combine(root, "Modbus RTU/00_gouvernance/Echanges_Codex/preuves", mission, name);
        using var bytes = File.OpenRead(path);
        using var z = new ZLibStream(bytes, CompressionMode.Decompress);
        using var reader = new StreamReader(z);
        var text = await reader.ReadToEndAsync();
        var adus = 0; var rejected = 0; var held = 0; var b0Count = 0;
        foreach (var line in text.Split('\n', StringSplitOptions.RemoveEmptyEntries))
        {
            using var doc = JsonDocument.Parse(line);
            var row = doc.RootElement;
            if (row.GetProperty("kind").GetString() != "adu") continue;
            adus++;
            var request = Convert.FromHexString(row.GetProperty("request").GetString()!.Replace(" ", "", StringComparison.Ordinal));
            if (request[1] != 3) continue;
            var response = Convert.FromHexString(row.GetProperty("response").GetString()!.Replace(" ", "", StringComparison.Ordinal));
            var words = ModbusRtuReadCapture.Decode(request, response);
            var address = (request[2] << 8) | request[3];
            if (address == 0)
            {
                b0Count++;
                Assert.Equal(2, words[6]); Assert.Equal(12, words[7]);
            }
            if (address != 3000) continue;
            var decoded = B3VibrationSupervision.Parse(words); // Exact normative offsets, no swapping.
            Assert.Equal(0u, decoded.LastUpdateTr2Seconds);
            if (words[3] == 240)
            {
                rejected++;
                Assert.Throws<InvalidDataException>(() => LiveB3Snapshot.Parse(words));
                Assert.Equal((ushort)240, decoded.SeverityGlobal);
                Assert.True(decoded.RmsGlobalMg > 0);
                if ((words[1] & 512) != 0) { held++; Assert.Equal(0, words[1] & 4); }
            }
            else Assert.False(LiveB3Snapshot.Parse(words).ValuesValid);
        }
        Assert.Equal(expected, adus); Assert.True(b0Count > 0); Assert.Equal(expected == 1632 ? 780 : expected == 1655 ? 790 : 498, rejected); Assert.True(held > 10);
    }

    [Fact]
    public async Task StrictSelectionPreservesV1AndRejectsUnknownAndPartialUnlessExplicit()
    {
        var t = new FakeTransport { Version = 1 };
        var reader = new LiveProfileReader(t);
        var legacy = await reader.IdentifyAsync(Endpoint);
        Assert.Equal(SupervisionProfile.LegacyV1, legacy.Profile);
        Assert.Single(t.Reads); // No speculative long B5 / epoch / write.
        await Assert.ThrowsAsync<NotSupportedException>(() => reader.ReadAsync(Endpoint, legacy).AsTask());
        t.Version = 3;
        await Assert.ThrowsAsync<InvalidDataException>(() => reader.IdentifyAsync(Endpoint).AsTask());
        t.Version = 2; t.Capabilities = 12;
        await Assert.ThrowsAsync<NotSupportedException>(() => reader.IdentifyAsync(Endpoint).AsTask());
        var diagnostic = await new LiveProfileReader(t, true).IdentifyAsync(Endpoint);
        Assert.Equal(SupervisionProfile.LiveDiagnostic, diagnostic.Profile);
        Assert.Null(diagnostic.DiagnosticStructureVersion);
        Assert.DoesNotContain(t.Reads, x => x is 1000 or 2000 or 5000 or 7000);
        t.Capabilities = 29;
        Assert.Equal(SupervisionProfile.Live, (await reader.IdentifyAsync(Endpoint)).Profile);
        t.ConfigurationVersion = 1;
        await Assert.ThrowsAsync<InvalidDataException>(() => reader.IdentifyAsync(Endpoint).AsTask());
        t.ConfigurationVersion = 2; t.InventoryVersion = 1;
        await Assert.ThrowsAsync<InvalidDataException>(() => reader.IdentifyAsync(Endpoint).AsTask());
        t.InventoryVersion = 2; t.DiagnosticVersion = 2;
        await Assert.ThrowsAsync<InvalidDataException>(() => reader.IdentifyAsync(Endpoint).AsTask());
        t.DiagnosticVersion = 1; t.Capabilities = 16;
        await Assert.ThrowsAsync<InvalidDataException>(() => reader.IdentifyAsync(Endpoint).AsTask());
        Assert.Equal(0, t.Writes);
    }

    [Fact]
    public void WordOrderUnitsUtcAndReservedQualityAreStrict()
    {
        var w = Words(0x12345678); w[14] = 0x0102; w[15] = 0x0304;
        var v = LiveB3Snapshot.Parse(w);
        Assert.Equal(0x12345678u, v.Value.CalculationSequence);
        Assert.Equal(0x01020304u, v.Value.RmsGlobalMg);
        Assert.Null(v.WindowUtc);
        w[0] = 3; w[1] = 255; w[3] = 1; w[4] = 0x6500; w[5] = 0x1234;
        Assert.Equal(DateTimeOffset.FromUnixTimeSeconds(0x65001234), LiveB3Snapshot.Parse(w).WindowUtc);
        var modifications = new Action<ushort[]>[] {
            a => a[1] |= 0x8000, a => a[1] |= 256, a => a[40] = 1,
            a => a[0] = 6, a => a[2] = 512, a => a[1] |= 4096,
            a => a[1] |= 512, a => a[1] &= unchecked((ushort)~2),
            a => a[1] &= unchecked((ushort)~64), a => a[30] = 5,
            a => a[1] |= 2048, a => a[1] &= unchecked((ushort)~128)
        };
        foreach (var change in modifications)
        {
            var invalid = w.ToArray(); change(invalid);
            Assert.Throws<InvalidDataException>(() => LiveB3Snapshot.Parse(invalid));
        }
        Assert.Throws<InvalidDataException>(() => LiveB3Snapshot.Parse(w[..47]));
        Assert.Throws<InvalidDataException>(() => LiveB3Snapshot.Parse(new ushort[49]));
    }

    [Fact]
    public void AdusRejectWrongUnitCrcLengthFunctionAndTruncation()
    {
        byte[] request = [1,3,0,0,0,1,0x84,0x0A];
        byte[] response = [1,3,2,0,0,0xB8,0x44];
        Assert.Equal(new ushort[] { 0 }, ModbusRtuReadCapture.Decode(request, response));
        var corrupt = response.ToArray(); corrupt[3] ^= 1;
        Assert.Throws<InvalidDataException>(() => ModbusRtuReadCapture.Decode(request, corrupt));
        var wrongUnit = response.ToArray(); wrongUnit[0] = 2;
        Assert.Throws<InvalidDataException>(() => ModbusRtuReadCapture.Decode(request, wrongUnit));
        Assert.Throws<InvalidDataException>(() => ModbusRtuReadCapture.Decode(request, response[..^1]));
        var function = request.ToArray(); function[1] = 16;
        Assert.Throws<InvalidDataException>(() => ModbusRtuReadCapture.Decode(function, response));
    }

    [Fact]
    public async Task SequenceRepeatsGapsSaturationRebootDisconnectAndRestartAreNotExactlyOnce()
    {
        var t = new FakeTransport(); var sink = new MemorySink(); var clock = new Clock(Now);
        var service = new LiveObservationService(t, sink, clock: clock);
        var first = await service.PollAsync(Endpoint);
        Assert.Equal(LiveContinuity.SegmentStart, first.Observation.Continuity);
        Assert.Null(first.Observation.RunId); Assert.Null(first.Observation.Epoch);
        Assert.Null(first.Observation.ConfigurationGeneration); Assert.Null(first.Observation.EventMonotonicMicroseconds);
        Assert.Equal(LiveContinuity.RepeatedSequence, (await service.PollAsync(Endpoint)).Observation.Continuity);
        t.Sequence = 2; Assert.Equal(LiveContinuity.Sequential, (await service.PollAsync(Endpoint)).Observation.Continuity);
        t.Sequence = 5; Assert.Equal(LiveContinuity.Gap, (await service.PollAsync(Endpoint)).Observation.Continuity);
        t.Sequence = uint.MaxValue; Assert.Equal(LiveContinuity.IndeterminateSaturation, (await service.PollAsync(Endpoint)).Observation.Continuity);
        t.Sequence = 1;
        var reboot = await service.PollAsync(Endpoint);
        Assert.Equal(LiveContinuity.RegressedSequence, reboot.Observation.Continuity);
        Assert.NotEqual(first.Observation.SegmentId, reboot.Observation.SegmentId);
        t.Fail = true;
        var failed = await service.PollAsync(Endpoint);
        Assert.False(failed.IsCurrent); Assert.Null(failed.Observation.Snapshot);
        Assert.Equal(LiveContinuity.CommunicationLost, failed.Observation.Continuity);
        t.Fail = false;
        var resumed = await service.PollAsync(Endpoint);
        Assert.Equal(LiveContinuity.SegmentStart, resumed.Observation.Continuity);
        Assert.NotEqual(reboot.Observation.SegmentId, resumed.Observation.SegmentId);
        var restarted = await new LiveObservationService(t, sink, clock: clock).PollAsync(Endpoint);
        Assert.NotEqual(resumed.Observation.SegmentId, restarted.Observation.SegmentId);
        Assert.False(service.Read(Now.AddSeconds(6)).Single().IsCurrent);
        Assert.True(service.Read(Now.AddSeconds(-1)).Single().ReceptionStale);
        Assert.Equal(9, sink.Items.Count);
        Assert.Equal(0, t.Writes);
    }

    [Fact]
    public async Task ArchiveFailureDoesNotReportSuccessOrPermanentlyBlockReads()
    {
        var t = new FakeTransport(); var sink = new MemorySink { Fail = true };
        var service = new LiveObservationService(t, sink);
        var first = await service.PollAsync(Endpoint);
        Assert.Equal("Failed", first.ArchiveState); Assert.NotNull(first.ArchiveError);
        sink.Fail = false; t.Sequence++;
        Assert.Equal("Persisted", (await service.PollAsync(Endpoint)).ArchiveState);
        Assert.Single(sink.Items);
    }

    [Fact]
    public async Task InvalidFrameIsNotArchivedAsAValidatedValueOrRetainedAsCurrent()
    {
        var t = new FakeTransport(); var sink = new MemorySink(); var service = new LiveObservationService(t, sink);
        Assert.True((await service.PollAsync(Endpoint)).IsCurrent);
        t.Corrupt = true;
        var bad = await service.PollAsync(Endpoint);
        Assert.Equal(LiveContinuity.Rejected, bad.Observation.Continuity);
        Assert.False(bad.IsCurrent); Assert.Null(bad.Observation.Snapshot);
        Assert.NotNull(bad.Observation.RejectedB3Registers);
        Assert.Equal(1, bad.Observation.RejectedB3Registers![40]);
        Assert.NotNull(bad.Observation.RejectedB3Value);
        Assert.Equal(2, sink.Items.Count);
    }

    [Fact]
    public async Task MigrationFromSchema6KeepsHistoricalRowsUnknownAndPersistsEveryLiveObservation()
    {
        Directory.CreateDirectory(_directory);
        var database = new SqliteDatabase(new SqlitePersistenceOptions(Path.Combine(_directory, "archive.db")));
        // Migrations1..6 unchanged: remove only the new additive table to construct an authentic v6 schema.
        using (var c = database.OpenConnection())
        {
            using var cmd = c.CreateCommand();
            cmd.CommandText = "DROP TABLE live_observation; PRAGMA user_version=6;"; cmd.ExecuteNonQuery();
        }
        // Seed a historical row before reopening/migration, with every required v6 numeric field.
        using (var oldConnection = new SqliteConnection($"Data Source={Path.Combine(_directory, "archive.db")}"))
        {
            oldConnection.Open();
            using var schema = oldConnection.CreateCommand();
            schema.CommandText = "PRAGMA table_info(b3_archive)";
            var columns = new List<string>();
            using (var reader = schema.ExecuteReader())
                while (reader.Read()) if (reader.GetInt32(3) == 1 && reader.GetInt32(5) == 0) columns.Add(reader.GetString(1));
            using var seed = oldConnection.CreateCommand();
            var values = columns.Select(n => n == "device_id" ? "42" : n == "rms_global_mg" ? "958" : n == "received_utc" ? "'2026-10-16T00:00:00Z'" : "0");
            seed.CommandText = $"INSERT INTO b3_archive({string.Join(',', columns)}) VALUES({string.Join(',', values)})";
            seed.ExecuteNonQuery();
        }
        var live = new LiveObservationService(new FakeTransport(), new SqliteLiveObservationSink(database), clock: new Clock(Now));
        await live.PollAsync(Endpoint); await live.PollAsync(Endpoint);
        using var connection = database.OpenConnection();
        using var command = connection.CreateCommand();
        command.CommandText = "SELECT COUNT(*) FROM b3_archive WHERE device_id=42 AND rms_global_mg=958";
        Assert.Equal(1L, command.ExecuteScalar());
        command.CommandText = "SELECT COUNT(*) FROM live_observation"; Assert.Equal(2L, command.ExecuteScalar());
        command.CommandText = "PRAGMA user_version"; Assert.Equal(7L, command.ExecuteScalar());
        command.CommandText = "SELECT payload_json FROM live_observation ORDER BY row_id DESC LIMIT 1";
        using var json = JsonDocument.Parse((string)command.ExecuteScalar()!);
        Assert.Equal(JsonValueKind.Null, json.RootElement.GetProperty("RunId").ValueKind);
        Assert.Equal(JsonValueKind.Null, json.RootElement.GetProperty("ConfigurationId").ValueKind);
        Assert.Equal("LIVE_READONLY", json.RootElement.GetProperty("Mode").GetString());
        Assert.Equal(0, json.RootElement.GetProperty("Snapshot").GetProperty("Value").GetProperty("LastUpdateTr2Seconds").GetInt32());
    }

    [Fact]
    public async Task CorruptAndUnavailableSqliteAreVisibleAndDoNotOverwriteExistingFile()
    {
        Directory.CreateDirectory(_directory); var file = Path.Combine(_directory, "corrupt.db");
        var bytes = Encoding.UTF8.GetBytes("not a sqlite database"); await File.WriteAllBytesAsync(file, bytes);
        var service = new LiveObservationService(new FakeTransport(), new SqliteLiveObservationSink(new SqliteDatabase(new SqlitePersistenceOptions(file))));
        Assert.Equal("Failed", (await service.PollAsync(Endpoint)).ArchiveState);
        Assert.Equal(bytes, File.ReadAllBytes(file));
        var impossible = Path.Combine(file, "nested.db");
        var blocked = new LiveObservationService(new FakeTransport(), new SqliteLiveObservationSink(new SqliteDatabase(new SqlitePersistenceOptions(impossible))));
        Assert.Equal("Failed", (await blocked.PollAsync(Endpoint)).ArchiveState);
    }

    [Fact]
    public async Task WebLiveReadOnlyRouteShowsQualityProvenanceAndS7StillWorks()
    {
        var clock = new Clock(Now); var transport = new FakeTransport();
        var source = new LiveObservationService(transport, new MemorySink(), clock: clock, source: "Simulated transport; not physical");
        await source.PollAsync(Endpoint);
        var host = new SupervisionWebHost(new SupervisionWebOptions(new Uri("http://127.0.0.1:0"), false),
            new SupervisionReadProjection(new FleetRegistry(), new DeviceTelemetrySnapshotRegistry(), new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10))), clock, liveReadSource: source);
        await using var app = host.CreateApplication(); await app.StartAsync();
        try
        {
            var address = app.Services.GetRequiredService<IServer>().Features.Get<IServerAddressesFeature>()!.Addresses.Single();
            using var client = new HttpClient { BaseAddress = new Uri(address) };
            var json = await client.GetStringAsync("/api/v2/live");
            Assert.Contains("Simulated transport; not physical", json);
            using var doc = JsonDocument.Parse(json);
            Assert.True(doc.RootElement[0].GetProperty("isCurrent").GetBoolean());
            Assert.Equal(JsonValueKind.Null, doc.RootElement[0].GetProperty("observation").GetProperty("runId").ValueKind);
            transport.Held = true; await source.PollAsync(Endpoint);
            using var held = JsonDocument.Parse(await client.GetStringAsync("/api/v2/live"));
            Assert.False(held.RootElement[0].GetProperty("isCurrent").GetBoolean());
            Assert.True(held.RootElement[0].GetProperty("observation").GetProperty("snapshot").GetProperty("held").GetBoolean());
            Assert.Contains("HELD", await client.GetStringAsync("/live.js"));
            Assert.Contains("lecture seule", await client.GetStringAsync("/live.html"));
            Assert.Contains("Vue générale", await client.GetStringAsync("/"));
            Assert.True((await client.GetAsync("/api/v1/fleet")).IsSuccessStatusCode);
            Assert.Equal(System.Net.HttpStatusCode.MethodNotAllowed, (await client.PostAsync("/api/v2/live", null)).StatusCode);
            Assert.Equal(System.Net.HttpStatusCode.NotFound, (await client.PostAsync("/api/v1/devices/42/commands", new StringContent("{\"requestIdentity\":\"test\",\"command\":\"SoftwareReset\"}", Encoding.UTF8, "application/json"))).StatusCode);
            clock.Now = Now.AddSeconds(6);
            using var stale = JsonDocument.Parse(await client.GetStringAsync("/api/v2/live"));
            Assert.True(stale.RootElement[0].GetProperty("receptionStale").GetBoolean());
            Assert.Equal(0, transport.Writes);
        }
        finally { await app.StopAsync(); }
    }

    internal static ushort[] Words(uint sequence)
    {
        ushort[] w = new ushort[48];
        w[0] = 4; w[1] = 1215; w[3] = 2;
        w[8] = (ushort)(sequence >> 16); w[9] = (ushort)sequence;
        w[11] = 154; w[13] = 4096; w[15] = 958; w[17] = 998;
        w[19] = 955; w[21] = 75; w[23] = 13; w[25] = 994; w[27] = 110; w[29] = 50; w[30] = 1;
        return w;
    }
    private sealed class FakeTransport : IRegisterTransport, IRegisterWriteTransport
    {
        public ushort Version = 2, Capabilities = 29, ConfigurationVersion = 2, InventoryVersion = 2, DiagnosticVersion = 1;
        public uint Sequence = 1;
        public bool Fail, Corrupt, Held;
        public int Writes;
        public List<ushort> Reads = [];
        public ValueTask<ushort[]> ReadRegistersAsync(string bus, byte unit, ushort start, ushort count, CancellationToken cancellationToken = default)
        {
            Reads.Add(start);
            if (Fail) throw new TimeoutException("Simulated transport lost");
            var words = new ushort[count];
            switch (start)
            {
                case 0: words[1] = 42; words[6] = Version; words[7] = Capabilities; break;
                case 4000: words[0] = ConfigurationVersion; words[1] = 1; break;
                case 6000: words[0] = InventoryVersion; break;
                case 7000: words[0] = DiagnosticVersion; break;
                case 3000:
                    words = Words(Sequence);
                    if (Held) { words[0] = 1; words[1] = 5819; words[3] = 0; }
                    if (Corrupt) words[40] = 1;
                    break;
                default: throw new InvalidOperationException("Unexpected read");
            }
            return ValueTask.FromResult(words);
        }
        public ValueTask WriteRegistersAsync(string busId, byte unitAddress, ushort startAddress, IReadOnlyList<ushort> values, CancellationToken cancellationToken = default)
        { Writes++; throw new InvalidOperationException("No write is authorized in LIVE readonly mode."); }
    }
    private sealed class MemorySink : ILiveObservationSink
    {
        public bool Fail;
        public List<LiveObservation> Items = [];
        public ValueTask AppendAsync(LiveObservation observation, CancellationToken ct = default)
        {
            if (Fail) throw new IOException("Simulated archive unavailable");
            Items.Add(observation); return ValueTask.CompletedTask;
        }
    }
    private sealed class Clock(DateTimeOffset value) : TimeProvider
    {
        public DateTimeOffset Now = value;
        public override DateTimeOffset GetUtcNow() => Now;
    }
    private static string FindRepository()
    {
        var d = new DirectoryInfo(AppContext.BaseDirectory);
        while (d is not null && !File.Exists(Path.Combine(d.FullName, "AGENTS.md"))) d = d.Parent;
        return d?.FullName ?? throw new DirectoryNotFoundException("Repository not found");
    }
    public void Dispose()
    {
        SqliteConnection.ClearAllPools();
        if (Directory.Exists(_directory)) Directory.Delete(_directory, true);
    }
}
