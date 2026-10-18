using System.Text.Json;
using TR2.Application;
using TR2.Domain;
using TR2.Persistence.Sqlite;
using TR2.Supervision.Web;
using TR2.Transport;

namespace TR2.Supervision.Service;

// Explicit partial-target opt-in, same NModbus/application/SQLite/Web composition as the existing service.
// No command capability is passed to the observation service or the Web host.
public static class LiveDirectConsoleApplication
{
    public static async Task<int> RunAsync(IReadOnlyList<string> args, CancellationToken ct, TextWriter output, TextWriter error)
    {
        if (args.Count != 15 || args[0] != "--live-direct" || args[2] != "--allow-diagnostic" ||
            args[3] != "--database" || args[5] != "--listen" || args[7] != "--trace" ||
            args[9] != "--seconds" || args[11] != "--unit" || args[13] != "--connection-control" ||
            !int.TryParse(args[10], out var seconds) || seconds is < 1 or > 600 ||
            !byte.TryParse(args[12], out var unit) || unit is < 1 or > 247 ||
            !Uri.TryCreate(args[6], UriKind.Absolute, out var listen) || listen.Scheme != "http" || !listen.IsLoopback)
        {
            await error.WriteLineAsync("Usage: --live-direct COM4 --allow-diagnostic --database <new.db> --listen http://127.0.0.1:port --trace <new.jsonl> --seconds 30 --unit 1 --connection-control <path> (file content disconnected closes COM; absent reconnects). 115200 8E1, readonly, NON-PRODUCTION.");
            return 2;
        }
        try
        {
            await using var trace = new FileStream(args[8], FileMode.CreateNew, FileAccess.Write, FileShare.Read);
            using var writer = new StreamWriter(trace) { AutoFlush = true };
            await using var transport = new LiveDirectTransport(new NModbusSerialBusConnectionFactory(
                row => writer.WriteLine(JsonSerializer.Serialize(row))), args[1]);
            var source = new LiveObservationService(transport,
                new SqliteLiveObservationSink(new SqliteDatabase(new SqlitePersistenceOptions(args[4]))),
                allowDiagnostic: true, source: $"Physical RS485 {args[1]} 115200 8E1 / NModbus; DIAGNOSTIC NON-PRODUCTION; actual B0/B4/B6/B3; no UTC/run/config inference");
            var host = new SupervisionWebHost(new SupervisionWebOptions(listen, false),
                new SupervisionReadProjection(new FleetRegistry(), new DeviceTelemetrySnapshotRegistry(),
                    new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10))), liveReadSource: source, liveArchiveReadSource: new SqliteLiveArchiveReadSource(args[4]), liveDataIsPhysical: true);
            await using var app = host.CreateApplication();
            await app.StartAsync(ct);
            await output.WriteLineAsync("Physical readonly LIVE diagnostic; one exclusive serial owner; no FC10/B5 writes. Open /live.html.");
            var endpoint = new TR2Endpoint(new SerialBus("live-direct"), new ModbusAddress(unit));
            var elapsed = System.Diagnostics.Stopwatch.StartNew();
            while (!ct.IsCancellationRequested && elapsed.Elapsed < TimeSpan.FromSeconds(seconds))
            {
                var disconnected = File.Exists(args[14]) && File.ReadAllText(args[14]).Trim() == "disconnected";
                await transport.SetConnectedAsync(!disconnected, ct);
                var view = await source.PollAsync(endpoint, ct);
                await output.WriteLineAsync(JsonSerializer.Serialize(view));
                await Task.Delay(TimeSpan.FromMilliseconds(100), ct);
            }
            await app.StopAsync(CancellationToken.None);
            return 0;
        }
        catch (OperationCanceledException) when (ct.IsCancellationRequested) { return 0; }
        catch (Exception ex) when (ex is IOException or ArgumentException or UnauthorizedAccessException or InvalidOperationException)
        {
            await error.WriteLineAsync($"Direct LIVE failed: {ex.Message}");
            return 1;
        }
    }
}

// Sole owner within this composition. Reconnect happens between complete observation cycles.
// Explicit logical disconnect closes the actual serial handle; no synthetic measurement is supplied.
public sealed class LiveDirectTransport(IModbusBusConnectionFactory factory, string port) : IRegisterTransport, IAsyncDisposable
{
    private IModbusBusConnection? _connection;
    private readonly SemaphoreSlim _gate = new(1, 1);
    private bool _enabled = true;
    private bool _disposed;
    public async ValueTask SetConnectedAsync(bool enabled, CancellationToken ct = default)
    {
        await _gate.WaitAsync(ct);
        try
        {
            ObjectDisposedException.ThrowIf(_disposed, this);
            _enabled = enabled;
            if (!enabled && _connection is not null)
            {
                await _connection.DisposeAsync();
                _connection = null;
            }
        }
        finally { _gate.Release(); }
    }
    public async ValueTask<ushort[]> ReadRegistersAsync(string busId, byte unitAddress, ushort startAddress, ushort registerCount, CancellationToken cancellationToken = default)
    {
        await _gate.WaitAsync(cancellationToken);
        try
        {
            ObjectDisposedException.ThrowIf(_disposed, this);
            if (!_enabled) throw new IOException("Operator logical disconnect: serial handle closed; no target read.");
            if (busId != "live-direct" || !((startAddress, registerCount) is (0, 21) or (3000, 48) or (4000, 2) or (6000, 1) or (7000, 1)))
                throw new InvalidDataException("Outside bounded readonly LIVE registers.");
            try
            {
                _connection ??= await factory.OpenAsync(busId, new(port, 115200, 8, ModbusSerialParity.Even, ModbusSerialStopBits.One, 1500), cancellationToken);
                return await _connection.RegisterTransport.ReadRegistersAsync(busId, unitAddress, startAddress, registerCount, cancellationToken);
            }
            catch (Exception ex) when (ex is IOException or TimeoutException or UnauthorizedAccessException or ModbusTransportFailureException)
            {
                if (_connection is not null) await _connection.DisposeAsync();
                _connection = null;
                throw new IOException("Physical serial read failed; handle released; no retry within observation.", ex);
            }
        }
        finally { _gate.Release(); }
    }
    public async ValueTask DisposeAsync()
    {
        await _gate.WaitAsync();
        try
        {
            if (_disposed) return;
            _disposed = true;
            if (_connection is not null) await _connection.DisposeAsync();
            _connection = null;
        }
        finally { _gate.Release(); }
    }
}
