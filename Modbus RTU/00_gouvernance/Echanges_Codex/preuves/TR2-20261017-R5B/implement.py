from pathlib import Path
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R5B-live-to-web');s=r/'Modbus RTU/Supervision TR2/src'
p=s/'TR2.Transport/NModbusSerialBusConnectionFactory.cs';t=p.read_text().replace('public sealed class NModbusSerialBusConnectionFactory : IModbusBusConnectionFactory','public sealed class NModbusSerialBusConnectionFactory(Action<SerialStreamTrace>? trace = null) : IModbusBusConnectionFactory').replace('var master = modbusFactory.CreateRtuMaster(serialPort);','var master = trace is null ? modbusFactory.CreateRtuMaster(serialPort) :\n                modbusFactory.CreateRtuMaster(new TracedStreamResource(new SerialPortAdapter(serialPort), trace));');p.write_text(t)
(s/'TR2.Transport/TracedStreamResource.cs').write_text('''using NModbus.IO;

namespace TR2.Transport;

public sealed record SerialStreamTrace(DateTimeOffset ReceivedAt, string Direction, string Hex);

// Transparent byte trace below NModbus: records actual chunks, including incomplete/error replies.
// The observer must be local and bounded. No additional serial owner, retry or frame synthesis.
public sealed class TracedStreamResource(IStreamResource inner, Action<SerialStreamTrace> trace) : IStreamResource
{
    public int InfiniteTimeout => inner.InfiniteTimeout;
    public int ReadTimeout { get => inner.ReadTimeout; set => inner.ReadTimeout = value; }
    public int WriteTimeout { get => inner.WriteTimeout; set => inner.WriteTimeout = value; }
    public void DiscardInBuffer() => inner.DiscardInBuffer();
    public int Read(byte[] buffer, int offset, int count)
    {
        var n = inner.Read(buffer, offset, count);
        trace(new(DateTimeOffset.UtcNow, "rx", Convert.ToHexString(buffer.AsSpan(offset, n))));
        return n;
    }
    public void Write(byte[] buffer, int offset, int count)
    {
        trace(new(DateTimeOffset.UtcNow, "tx", Convert.ToHexString(buffer.AsSpan(offset, count))));
        inner.Write(buffer, offset, count);
    }
    public void Dispose() => inner.Dispose();
}
''')
(s/'TR2.Supervision.Service/LiveDirectConsoleApplication.cs').write_text('''using System.Text.Json;
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
                    new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10))), liveReadSource: source);
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
''')
p=s/'TR2.Supervision.Service/SupervisionConsoleApplication.cs';t=p.read_text();pos=t.index('        if (args.Count > 0 && args[0] == "--live-replay")');t=t[:pos]+'''        if (args.Count > 0 && args[0] == "--live-direct")
            return await LiveDirectConsoleApplication.RunAsync(args, cancellationToken, output, error);

'''+t[pos:];p.write_text(t)
f=r/'Modbus RTU/05_Firmware/platform/stm32';p=f/'CMakeLists.txt';t=p.read_text().replace('option(TR2_STM32_R5AFIX_B3', '''option(TR2_STM32_R5B_LIVE "R5B direct PC LIVE demo; NON-PRODUCTION" OFF)
if(TR2_STM32_R5B_LIVE AND (NOT TR2_STM32_R4U_RUNTIME OR TR2_STM32_R5AFIX_B3 OR TR2_STM32_R4V_LATENCY))
 message(FATAL_ERROR "R5B requires explicit R4U and excludes historical R5AFIX/R4V recipes")
endif()
option(TR2_STM32_R5AFIX_B3''').replace('set(profile_description', '''if(TR2_STM32_R5B_LIVE)
 set(TR2_ARTIFACT_NAME "tr2-r5b-live-to-web-NON-PRODUCTION")
endif()
set(profile_description''').replace('  if(TR2_STM32_R5AFIX_B3)\n   target_sources', '''  if(TR2_STM32_R5B_LIVE)
   target_sources(tr2_stm32_p11c PRIVATE stm32_r5b_live.c fifo_event_time.c)
   set_source_files_properties(stm32_r5b_live.c PROPERTIES COMPILE_OPTIONS "-O2")
  elseif(TR2_STM32_R5AFIX_B3)
   target_sources''');p.write_text(t)
p=f/'stm32_r4u_runtime.c';t=p.read_text().replace('#ifndef R4U_INITIAL_SHA','#ifndef R4U_STOP_HOLD_US\n#define R4U_STOP_HOLD_US 2000000\n#endif\n#ifndef R4U_INITIAL_SHA',1).replace('>2000000)break;', '>R4U_STOP_HOLD_US)break;');p.write_text(t)
t=(f/'stm32_r5afix_b3.c').read_text();t=t.replace('R5-AFIX only: exact monotone R4V final snapshot','R5-B only: exact monotone R5AFIX final snapshot').replace('START5/10/run10, STOP5/11','START5/12/run11, STOP5/13').replace('r5afix_prepared','r5b_prepared');sha='4e7fabde5da1100cf9ae8faf262ea079727856826bb2829c1e99b68ad63b84bd';t='\n'.join(('#define R4U_INITIAL_SHA '+','.join('0x'+sha[i:i+2] for i in range(0,64,2))) if x.startswith('#define R4U_INITIAL_SHA') else x for x in t.splitlines())+'\n';
for a,b in [('JOURNAL 62','JOURNAL 70'),('ROOT 75','ROOT 83'),('INITIAL_RUN 9','INITIAL_RUN 10'),('INITIAL_STOP 9','INITIAL_STOP 11'),('FINAL_RUN 10','FINAL_RUN 11'),('FINAL_STOP 11','FINAL_STOP 13')]:t=t.replace(a,b)
t=t.replace('#include "stm32_r4u_runtime.c"','#define R4U_STOP_HOLD_US 20000000 /* bounded HELD service observation after command-master closes */\n#include "stm32_r4u_runtime.c"');(f/'stm32_r5b_live.c').write_text(t)
