using System.Globalization;
using System.Text;
using System.Text.Json;
using TR2.Transport;

namespace TR2.Supervision.Service;

/// <summary>Isolated bench probe; never composes a production runtime or session.</summary>
public static class DiagnosticConsoleApplication
{
    private const string BusId = "tr2-bench-diagnostic";

    public static async Task<int> RunAsync(
        IReadOnlyList<string> args,
        CancellationToken cancellationToken,
        TextWriter output,
        TextWriter error,
        IModbusBusConnectionFactory? connectionFactory = null)
    {
        Options options;
        try
        {
            options = Parse(args);
        }
        catch (ArgumentException exception)
        {
            await error.WriteLineAsync($"DIAGNOSTIC usage error: {exception.Message}");
            await error.WriteLineAsync(
                "--diagnostic [--read-once] --port <name> --baud <rate> " +
                "--parity <None|Odd|Even|Mark|Space> --timeout-ms <100..5000> " +
                "[--log <new.jsonl>] (no --read-once: OFFLINE; no port opened)");
            return SupervisionConsoleApplication.UsageOrConfigurationExitCode;
        }

        await output.WriteLineAsync(
            "DIAGNOSTIC bench only: unit=1; FC03; start=1000; count=1; expected=UNKNOWN(0). " +
            "Production not evaluated; DeviceId unavailable; no polling, discovery or writes.");
        if (options.Settings is { } settings)
        {
            await output.WriteLineAsync($"Diagnostic serial plan: port={settings.PortName}; " +
                $"serial={settings.BaudRate}/8/{settings.Parity}/One; " +
                $"timeoutMs={settings.ResponseTimeoutMilliseconds}; retries=0; " +
                $"readDeadlineMs={settings.ResponseTimeoutMilliseconds * 4}.");
        }
        if (!options.ReadOnce)
        {
            await output.WriteLineAsync("OFFLINE: no port opened, no transmission, no log created.");
            return SupervisionConsoleApplication.SuccessExitCode;
        }

        // CreateNew and exclusive sharing prevent accidental overwriting/reuse of a probe log.
        // Persist the intent before opening the connection, then before the single read.
        FileStream? log = null;
        var stage = "journal";
        try
        {
            cancellationToken.ThrowIfCancellationRequested();
            log = new FileStream(options.LogPath!, FileMode.CreateNew, FileAccess.Write, FileShare.None);
            await RecordAsync(log, new
            {
                @event = "start", mode = "DIAGNOSTIC", utc = DateTimeOffset.UtcNow,
                options.Settings, unit = 1, function = 3, start = 1000, count = 1,
                retries = 0, operationDeadlineMs = options.Settings!.ResponseTimeoutMilliseconds * 4,
                expectedRequestHex = "010303E80001047A", expectedResponseHex = "0103020000B844",
                evidence = "decoded registers via NModbus; expected hex is not a captured wire frame"
            });
            stage = "open";
            await using (var connection = await (connectionFactory ?? new NModbusSerialBusConnectionFactory())
                .OpenAsync(BusId, options.Settings!, cancellationToken))
            {
                if (connection.BusId != BusId)
                {
                    throw new InvalidDataException("Diagnostic connection bus mismatch.");
                }

                cancellationToken.ThrowIfCancellationRequested();
                stage = "read";
                await RecordAsync(log, new { @event = "read_intent", utc = DateTimeOffset.UtcNow, maximumRequests = 1 });
                var read = connection.RegisterTransport.ReadRegistersAsync(
                    BusId, 1, 1000, 1, cancellationToken).AsTask();
                // WaitAsync bounds the caller even for a stalled driver or repeated ACK frames.
                // Disposal closes the serial resource on timeout/cancellation. Observe late faults
                // without launching another operation or pretending cancellation rolls back TX.
                _ = read.ContinueWith(task => { _ = task.Exception; }, CancellationToken.None,
                    TaskContinuationOptions.OnlyOnFaulted | TaskContinuationOptions.ExecuteSynchronously,
                    TaskScheduler.Default);
                var registers = await read.WaitAsync(
                    TimeSpan.FromMilliseconds(options.Settings!.ResponseTimeoutMilliseconds * 4),
                    cancellationToken);
                await RecordAsync(log, new { @event = "decoded_response", utc = DateTimeOffset.UtcNow, registers });
                if (registers.Length != 1 || registers[0] != 0)
                {
                    throw new InvalidDataException("Expected exactly one register containing UNKNOWN(0).");
                }
                stage = "close";
            }

            await RecordAsync(log, new { @event = "result", utc = DateTimeOffset.UtcNow, passed = true, value = "UNKNOWN(0)" });
            await output.WriteLineAsync("DIAGNOSTIC response: UNKNOWN(0); connection closed; no retry. " +
                "Production not evaluated, DeviceId unavailable.");
            return SupervisionConsoleApplication.SuccessExitCode;
        }
        catch (Exception exception)
        {
            var failure = new
            {
                @event = "failure", utc = DateTimeOffset.UtcNow, stage,
                type = exception.GetType().FullName, exception.Message,
                kind = (exception as ModbusTransportFailureException)?.Kind.ToString(),
                function = (exception as ModbusTransportFailureException)?.FunctionCode,
                exceptionCode = (exception as ModbusTransportFailureException)?.ExceptionCode,
                passed = false, transmissionMayHaveOccurred = stage is "read" or "close"
            };
            if (log is not null)
            {
                try { await RecordAsync(log, failure); }
                catch (Exception journalException)
                {
                    await error.WriteLineAsync($"DIAGNOSTIC journal failure: {journalException.Message}");
                }
            }
            await error.WriteLineAsync($"DIAGNOSTIC failure ({stage}): {JsonSerializer.Serialize(failure)}; no retry.");
            return SupervisionConsoleApplication.RuntimeFailureExitCode;
        }
        finally
        {
            if (log is not null) { await log.DisposeAsync(); }
        }
    }

    private static async Task RecordAsync(FileStream log, object entry)
    {
        await log.WriteAsync(Encoding.UTF8.GetBytes(JsonSerializer.Serialize(entry) + "\n"));
        log.Flush(flushToDisk: true);
    }

    private sealed record Options(bool ReadOnce, ModbusSerialConnectionSettings? Settings, string? LogPath);

    private static Options Parse(IReadOnlyList<string> args)
    {
        if (args.Count == 0 || args[0] != "--diagnostic")
        {
            throw new ArgumentException("Explicit --diagnostic entry required.");
        }

        var readOnce = false;
        var values = new Dictionary<string, string>(StringComparer.Ordinal);
        for (var index = 1; index < args.Count; index++)
        {
            var name = args[index];
            if (name == "--read-once" && !readOnce) { readOnce = true; continue; }
            if (name is not ("--port" or "--baud" or "--parity" or "--timeout-ms" or "--log") ||
                index + 1 == args.Count || string.IsNullOrWhiteSpace(args[index + 1]) ||
                args[index + 1].StartsWith("--", StringComparison.Ordinal) ||
                !values.TryAdd(name, args[++index]))
            {
                throw new ArgumentException($"Unknown, duplicate or incomplete option: {name}.");
            }
        }

        if (!readOnce && values.Count == 0) { return new Options(false, null, null); }
        if (!values.TryGetValue("--port", out var port) ||
            !values.TryGetValue("--baud", out var baudText) ||
            !int.TryParse(baudText, NumberStyles.None, CultureInfo.InvariantCulture, out var baud) || baud is < 1 or > 1000000 ||
            !values.TryGetValue("--parity", out var parityText) ||
            !Enum.GetNames<ModbusSerialParity>().Contains(parityText, StringComparer.Ordinal) ||
            !Enum.TryParse<ModbusSerialParity>(parityText, out var parity) ||
            !values.TryGetValue("--timeout-ms", out var timeoutText) ||
            !int.TryParse(timeoutText, NumberStyles.None, CultureInfo.InvariantCulture, out var timeout) || timeout is < 100 or > 5000)
        {
            throw new ArgumentException("Explicit port, baud (1..1000000), named parity and timeout (100..5000 ms) required.");
        }
        values.TryGetValue("--log", out var logPath);
        if (readOnce != (logPath is not null))
        {
            throw new ArgumentException("--read-once requires --log; OFFLINE does not create a log.");
        }
        return new Options(readOnce, new ModbusSerialConnectionSettings(port, baud, 8, parity,
            ModbusSerialStopBits.One, timeout), logPath);
    }
}
