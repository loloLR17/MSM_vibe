using System.Globalization;
using System.Text;
using System.Text.Json;
using TR2.Domain;
using TR2.Protocol;
using TR2.Transport;

namespace TR2.Supervision.Service;

/// <summary>Bounded read-only B0/B1 qualification; does not start operational polling.</summary>
public static class SystemBlockConsoleApplication
{
    private const string BusId = "tr2-system-check";

    public static async Task<int> RunAsync(IReadOnlyList<string> args,
        CancellationToken cancellationToken, TextWriter output, TextWriter error,
        IModbusBusConnectionFactory? factory = null)
    {
        DiagnosticConsoleApplication.Options options;
        ushort version;
        try
        {
            if (args.Count == 0 || args[0] != "--system-check")
                throw new ArgumentException("Explicit --system-check required.");
            var serialArgs = new List<string> { "--diagnostic" };
            string? versionText = null;
            for (var i = 1; i < args.Count; i++)
            {
                if (args[i] != "--protocol-version") { serialArgs.Add(args[i]); continue; }
                if (versionText is not null || ++i == args.Count)
                    throw new ArgumentException("One explicit protocol version required.");
                versionText = args[i];
            }
            if (!ushort.TryParse(versionText, NumberStyles.None, CultureInfo.InvariantCulture, out version))
                throw new ArgumentException("Explicit --protocol-version <0..65535> required; no assumed device identity/version.");
            options = DiagnosticConsoleApplication.Parse(serialArgs);
        }
        catch (ArgumentException exception)
        {
            await error.WriteLineAsync("SYSTEM_CHECK usage error: " + exception.Message);
            return SupervisionConsoleApplication.UsageOrConfigurationExitCode;
        }

        await output.WriteLineAsync($"SYSTEM_CHECK plan: unit=1; FC03 B0=0/21 then B1=1000/20 only after compatible B0; supportedProtocolVersion={version}; maximumRequests=2; retries=0; no writes/polling/runtime.");
        if (options.Settings is { } settings)
            await output.WriteLineAsync($"Serial plan: {settings.PortName}/{settings.BaudRate}/8/{settings.Parity}/One; timeoutMs={settings.ResponseTimeoutMilliseconds}; perReadDeadlineMs={settings.ResponseTimeoutMilliseconds * 4}.");
        if (!options.ReadOnce)
        {
            await output.WriteLineAsync("OFFLINE: no port opened, no transmission, no log created.");
            return SupervisionConsoleApplication.SuccessExitCode;
        }

        FileStream? log = null;
        var stage = "journal";
        var readsStarted = 0;
        try
        {
            cancellationToken.ThrowIfCancellationRequested();
            log = new FileStream(options.LogPath!, FileMode.CreateNew, FileAccess.Write, FileShare.None);
            await RecordAsync(log, new { @event = "start", utc = DateTimeOffset.UtcNow,
                mode = "SYSTEM_CHECK", options.Settings, supportedProtocolVersion = version,
                unit = 1, maximumRequests = 2, retries = 0,
                evidence = "decoded registers; not a captured wire frame; physical readiness is external" });
            stage = "open";
            uint deviceId;
            B1SystemState state;
            await using (var connection = await (factory ?? new NModbusSerialBusConnectionFactory())
                .OpenAsync(BusId, options.Settings!, cancellationToken))
            {
                if (connection.BusId != BusId) throw new InvalidDataException("System check bus mismatch.");
                var endpoint = new TR2Endpoint(new SerialBus(BusId), new ModbusAddress(1));
                stage = "B0";
                var b0 = await ReadAsync(B0Reader.StartAddress, B0Identification.RegisterCount);
                var session = B0SessionFactory.Create(endpoint, b0, version);
                if (session.State != TR2SessionState.Compatible || session.Device is null)
                    throw new InvalidDataException("B0 protocol incompatible: no B1 request permitted.");
                deviceId = session.Device.DeviceId.Value;
                await RecordAsync(log, new { @event = "identification", utc = DateTimeOffset.UtcNow,
                    deviceId, protocolVersion = B0Identification.Parse(b0).ProtocolVersion });
                stage = "B1";
                var b1 = await ReadAsync(B1SystemState.StartAddress, B1SystemState.RegisterCount);
                state = B1SystemState.Parse(b1);
                stage = "close";

                async Task<ushort[]> ReadAsync(ushort address, ushort count)
                {
                    cancellationToken.ThrowIfCancellationRequested();
                    await RecordAsync(log, new { @event = "read_intent", utc = DateTimeOffset.UtcNow,
                        function = 3, start = address, count });
                    readsStarted++;
                    var read = connection.RegisterTransport.ReadRegistersAsync(BusId, 1, address, count, cancellationToken).AsTask();
                    _ = read.ContinueWith(task => { _ = task.Exception; }, CancellationToken.None,
                        TaskContinuationOptions.OnlyOnFaulted | TaskContinuationOptions.ExecuteSynchronously, TaskScheduler.Default);
                    var registers = await read.WaitAsync(TimeSpan.FromMilliseconds(options.Settings!.ResponseTimeoutMilliseconds * 4), cancellationToken);
                    await RecordAsync(log, new { @event = "decoded_response", utc = DateTimeOffset.UtcNow, start = address, registers });
                    if (registers.Length != count) throw new InvalidDataException($"Expected exactly {count} registers at {address}.");
                    return registers;
                }
            }
            await RecordAsync(log, new { @event = "result", utc = DateTimeOffset.UtcNow, passed = true,
                deviceId, systemState = state, readsStarted });
            await output.WriteLineAsync($"SYSTEM_CHECK complete: DeviceId={deviceId}; SystemStatus={state.SystemStatus}; connection closed; no retry. Status is decoded, not an assertion of READY or production qualification.");
            return SupervisionConsoleApplication.SuccessExitCode;
        }
        catch (Exception exception)
        {
            var failure = new { @event = "failure", utc = DateTimeOffset.UtcNow, stage,
                type = exception.GetType().FullName, exception.Message, readsStarted,
                kind = (exception as ModbusTransportFailureException)?.Kind.ToString(),
                exceptionCode = (exception as ModbusTransportFailureException)?.ExceptionCode,
                passed = false, transmissionMayHaveOccurred = readsStarted != 0 };
            if (log is not null)
            {
                try { await RecordAsync(log, failure); }
                catch (Exception journalException) { await error.WriteLineAsync("SYSTEM_CHECK journal failure: " + journalException.Message); }
            }
            await error.WriteLineAsync("SYSTEM_CHECK failure: " + JsonSerializer.Serialize(failure) + "; no retry.");
            return SupervisionConsoleApplication.RuntimeFailureExitCode;
        }
        finally { if (log is not null) await log.DisposeAsync(); }
    }

    private static async Task RecordAsync(FileStream log, object record)
    {
        await log.WriteAsync(Encoding.UTF8.GetBytes(JsonSerializer.Serialize(record) + "\n"));
        log.Flush(flushToDisk: true);
    }
}
