using System.Text.Json;
using TR2.Application;
using TR2.Domain;
using TR2.Persistence.Sqlite;
using TR2.Supervision.Web;
using TR2.Transport;

namespace TR2.Supervision.Service;

// Explicit offline mode, composed with the existing application, SQLite and S7 Web host.
public static class LiveReplayConsoleApplication
{
    public static async Task<int> RunAsync(IReadOnlyList<string> args, CancellationToken ct, TextWriter output, TextWriter error)
    {
        if (args.Count != 6 || args[2] != "--database" || args[4] != "--listen")
        {
            await error.WriteLineAsync("Usage: --live-replay <capture.jsonl> --database <temporary.db> --listen http://127.0.0.1:port");
            return 2;
        }
        try
        {
            var transport = new CapturedLiveTransport(args[1]);
            var source = new LiveObservationService(transport,
                new SqliteLiveObservationSink(new SqliteDatabase(new SqlitePersistenceOptions(args[3]))),
                allowDiagnostic: true, source: "Offline ADU replay; B4/B6 structure headers simulated; no serial access");
            var host = new SupervisionWebHost(new SupervisionWebOptions(new Uri(args[5]), false),
                new SupervisionReadProjection(new FleetRegistry(), new DeviceTelemetrySnapshotRegistry(),
                    new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10))), liveReadSource: source, liveArchiveReadSource: new SqliteLiveArchiveReadSource(args[3]), liveDataIsPhysical: false);
            await using var app = host.CreateApplication();
            await app.StartAsync(ct);
            await output.WriteLineAsync("OFFLINE replay only: historical B0/B3 ADUs, simulated structure headers, no COM/STM32. Open /live.html.");
            var endpoint = new TR2Endpoint(new SerialBus("offline-replay"), new ModbusAddress(transport.Unit));
            while (!ct.IsCancellationRequested)
            {
                await source.PollAsync(endpoint, ct);
                await Task.Delay(TimeSpan.FromMilliseconds(100), ct);
            }
            return 0;
        }
        catch (OperationCanceledException) when (ct.IsCancellationRequested) { return 0; }
        catch (Exception ex) when (ex is IOException or ArgumentException or JsonException or FormatException)
        {
            await error.WriteLineAsync($"Offline replay failed: {ex.Message}");
            return 1;
        }
    }
}

public sealed class CapturedLiveTransport : IRegisterTransport
{
    private readonly ushort[] _b0;
    private readonly Queue<ushort[]> _b3 = new();
    public byte Unit { get; }
    public CapturedLiveTransport(string path)
    {
        ushort[]? b0 = null;
        byte? unit = null;
        foreach (var line in File.ReadLines(path))
        {
            using var document = JsonDocument.Parse(line);
            var row = document.RootElement;
            if (row.GetProperty("kind").GetString() != "adu") continue;
            var request = Convert.FromHexString(row.GetProperty("request").GetString()!.Replace(" ", "", StringComparison.Ordinal));
            if (request.Length < 4 || request[1] != 3) continue;
            var response = Convert.FromHexString(row.GetProperty("response").GetString()!.Replace(" ", "", StringComparison.Ordinal));
            var words = ModbusRtuReadCapture.Decode(request, response);
            if (unit is not null && unit != request[0]) throw new InvalidDataException("Mixed units in capture.");
            unit = request[0];
            var address = (request[2] << 8) | request[3];
            if (address == 0)
            {
                if (words.Length != 21 || (b0 is not null && !b0.SequenceEqual(words)))
                    throw new InvalidDataException("Changed or malformed B0 in capture.");
                b0 = words;
            }
            if (address == 3000) _b3.Enqueue(words);
        }
        _b0 = b0 ?? throw new InvalidDataException("Missing B0.");
        Unit = unit ?? throw new InvalidDataException("Missing unit.");
        if (_b3.Count == 0) throw new InvalidDataException("Missing B3.");
    }
    public ValueTask<ushort[]> ReadRegistersAsync(string busId, byte unitAddress, ushort startAddress, ushort registerCount, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();
        if (unitAddress != Unit) throw new InvalidDataException("Wrong capture unit.");
        ushort[] words = (startAddress, registerCount) switch
        {
            (0, 21) => _b0.ToArray(),
            (4000, 2) => [2, 1], // Fixture, not a captured hardware observation.
            (6000, 1) => [2],
            (3000, 48) => _b3.Count != 0 ? _b3.Dequeue() : throw new EndOfStreamException("Replay exhausted; no current acquisition."),
            _ => throw new InvalidDataException("Read outside the bounded offline LIVE path.")
        };
        return ValueTask.FromResult(words);
    }
}
