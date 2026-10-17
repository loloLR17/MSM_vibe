using System.Text.Json;
using System.IO.Compression;
using Microsoft.Data.Sqlite;
using TR2.Application;
using TR2.Domain;
using TR2.Persistence.Sqlite;
using TR2.Protocol;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class B3WireOrderFixTests
{
    [Fact]
    public async Task CorrectedCProjectorFixturesMatchIndependentNormativeOffsetsAndPersistWithoutQuarantine()
    {
        var expected = new Dictionary<string, (ushort Alarms, ushort Severity)>
        {
            ["boot_invalid"] = (0, 0), ["waiting"] = (0, 0), ["normal_utc_fresh"] = (0, 1),
            ["fresh_utc_absent"] = (0, 2), ["warning_exceed"] = (0xF0, 3),
            ["active_alarm_latch"] = (0x1FF, 4), ["held_stop"] = (0x1FF, 0),
            ["latch_without_active_alarm"] = (0x100, 2), ["calculation_error_held"] = (0x100, 0)
        };
        var dir = new DirectoryInfo(AppContext.BaseDirectory);
        while (dir is not null && !File.Exists(Path.Combine(dir.FullName, "AGENTS.md"))) dir = dir.Parent;
        var file = Path.Combine(dir!.FullName, "Modbus RTU/Supervision TR2/tests/fixtures/r5afix-positive-adu.jsonl");
        var frames = new Queue<ushort[]>();
        foreach (var line in File.ReadLines(file))
        {
            using var document = JsonDocument.Parse(line);
            var row = document.RootElement;
            var request = Convert.FromHexString(row.GetProperty("request").GetString()!.Replace(" ", ""));
            var response = Convert.FromHexString(row.GetProperty("response").GetString()!.Replace(" ", ""));
            var words = ModbusRtuReadCapture.Decode(request, response);
            var name = row.GetProperty("case").GetString()!;
            var b3 = LiveB3Snapshot.Parse(words);
            Assert.Equal(expected[name].Alarms, b3.Value.AlarmFlags);
            Assert.Equal(expected[name].Severity, b3.Value.SeverityGlobal);
            if (name == "held_stop") { Assert.True(b3.Held); Assert.False(b3.Fresh); }
            if (name == "fresh_utc_absent") { Assert.True(b3.ValuesValid); Assert.True(b3.Fresh); Assert.Null(b3.WindowUtc); }
            if (name == "calculation_error_held") Assert.False(b3.ValuesValid);
            frames.Enqueue(words);
        }
        Assert.Equal(9, frames.Count);
        var path = Path.Combine(Path.GetTempPath(), "tr2-r5afix-" + Guid.NewGuid(), "fixture.db");
        try
        {
            var database = new SqliteDatabase(new SqlitePersistenceOptions(path));
            var source = new LiveObservationService(new FixtureTransport(frames), new SqliteLiveObservationSink(database),
                allowDiagnostic: true, source: "R5AFIX C host-model fixture; not a physical acquisition");
            var endpoint = new TR2Endpoint(new SerialBus("fixture"), new ModbusAddress(1));
            for (var i = 0; i < 9; i++)
            {
                var observation = await source.PollAsync(endpoint);
                Assert.NotEqual(LiveContinuity.Rejected, observation.Observation.Continuity);
                Assert.Equal("Persisted", observation.ArchiveState);
            }
            using var connection = database.OpenConnection();
            using var command = connection.CreateCommand();
            command.CommandText = "SELECT COUNT(*) FROM live_observation WHERE continuity='Rejected'";
            Assert.Equal(0L, command.ExecuteScalar());
            command.CommandText = "SELECT COUNT(*) FROM live_observation WHERE values_valid=1 AND fresh=1";
            Assert.Equal(5L, command.ExecuteScalar());
        }
        finally
        {
            SqliteConnection.ClearAllPools();
            Directory.Delete(Path.GetDirectoryName(path)!, true);
        }
    }

    [Fact]
    public async Task NewPhysicalCaptureIsAcceptedByUnmodifiedReaderAndSqliteWhileOldCapturesStayNegative()
    {
        var root = new DirectoryInfo(AppContext.BaseDirectory);
        while (root is not null && !File.Exists(Path.Combine(root.FullName, "AGENTS.md"))) root = root.Parent;
        var path = Path.Combine(root!.FullName, "Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261017-R5AFIX/hardware/fix-master.jsonl.zlib");
        using var compressed = File.OpenRead(path);
        using var stream = new ZLibStream(compressed, CompressionMode.Decompress);
        using var reader = new StreamReader(stream);
        var text = await reader.ReadToEndAsync();
        var count = 0;
        foreach (var line in text.Split('\n', StringSplitOptions.RemoveEmptyEntries))
        {
            using var json = JsonDocument.Parse(line);
            var row = json.RootElement;
            if (row.GetProperty("kind").GetString() != "adu") continue;
            var request = Convert.FromHexString(row.GetProperty("request").GetString()!.Replace(" ", ""));
            if (request[1] == 3 && request[2] == 11 && request[3] == 184) count++;
        }
        Assert.True(count > 10);
        var directory = Path.Combine(Path.GetTempPath(), "tr2-r5afix-physical-replay-" + Guid.NewGuid());
        Directory.CreateDirectory(directory);
        var capture = Path.Combine(directory, "capture.jsonl");
        await File.WriteAllTextAsync(capture, text);
        try
        {
            var database = new SqliteDatabase(new SqlitePersistenceOptions(Path.Combine(directory, "replay.db")));
            var source = new LiveObservationService(new CapturedLiveTransport(capture), new SqliteLiveObservationSink(database),
                allowDiagnostic: true, source: "Historical R5AFIX physical B0/B3; offline replay; B4/B6 headers simulated");
            var endpoint = new TR2Endpoint(new SerialBus("offline"), new ModbusAddress(1));
            var valid = 0; var held = 0; var nonzeroAlarm = false;
            for (var i = 0; i < count; i++)
            {
                var observation = await source.PollAsync(endpoint);
                Assert.NotEqual(LiveContinuity.Rejected, observation.Observation.Continuity);
                Assert.Equal("Persisted", observation.ArchiveState);
                Assert.Equal(SupervisionProfile.LiveDiagnostic, observation.Observation.Identity!.Profile);
                var snapshot = observation.Observation.Snapshot!;
                Assert.Null(snapshot.WindowUtc);
                if (snapshot.ValuesValid && snapshot.Fresh) valid++;
                if (snapshot.Held) { held++; Assert.False(observation.IsCurrent); }
                nonzeroAlarm |= snapshot.Value.AlarmFlags != 0;
                Assert.Null(observation.Observation.RunId);
            }
            Assert.True(valid > 0); Assert.True(held > 0); Assert.True(nonzeroAlarm);
            using var connection = database.OpenConnection();
            using var command = connection.CreateCommand();
            command.CommandText = "SELECT COUNT(*) FROM live_observation"; Assert.Equal((long)count, command.ExecuteScalar());
            command.CommandText = "SELECT COUNT(*) FROM live_observation WHERE continuity='Rejected'"; Assert.Equal(0L, command.ExecuteScalar());
        }
        finally { SqliteConnection.ClearAllPools(); Directory.Delete(directory, true); }
    }

    private sealed class FixtureTransport(Queue<ushort[]> frames) : IRegisterTransport
    {
        public ValueTask<ushort[]> ReadRegistersAsync(string busId, byte unitAddress, ushort startAddress, ushort registerCount, CancellationToken cancellationToken = default)
        {
            if (startAddress == 3000) return ValueTask.FromResult(frames.Dequeue());
            if (startAddress == 4000) return ValueTask.FromResult<ushort[]>([2, 1]);
            if (startAddress == 6000) return ValueTask.FromResult<ushort[]>([2]);
            var b0 = new ushort[21]; b0[1] = 1; b0[6] = 2; b0[7] = 12;
            Assert.Equal((ushort)0, startAddress);
            return ValueTask.FromResult(b0);
        }
    }
}
