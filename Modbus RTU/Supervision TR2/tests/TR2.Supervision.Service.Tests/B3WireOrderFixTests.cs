using System.Text.Json;
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
