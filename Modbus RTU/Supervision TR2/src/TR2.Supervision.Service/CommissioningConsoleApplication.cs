using System.Globalization;
using System.Text;
using System.Text.Json;
using TR2.Application;
using TR2.Domain;
using TR2.Persistence.Sqlite;
using TR2.Protocol;
using TR2.Transport;

namespace TR2.Supervision.Service;

/// <summary>Finite M014-G B0/B1 reads, one planned timeout, and explicit fresh reconnect.</summary>
public static class CommissioningConsoleApplication
{
    private const string BusId = "tr2-m014-g";
    private const int PollsPerPhase = 3;
    private const int MaximumRequests = 9;
    private static readonly TimeSpan Interval = TimeSpan.FromSeconds(2);
    private static readonly TimeSpan LossDelay = TimeSpan.FromSeconds(100);
    private static readonly TimeSpan RecoveryDelay = TimeSpan.FromSeconds(100);

    public static async Task<int> RunAsync(IReadOnlyList<string> args,
        CancellationToken token, TextWriter output, TextWriter error,
        IModbusBusConnectionFactory? factory = null,
        Func<TimeSpan, CancellationToken, Task>? wait = null)
    {
        DiagnosticConsoleApplication.Options options;
        ushort protocol;
        ushort[] expected;
        try
        {
            if (args.Count == 0 || args[0] != "--commissioning-check")
                throw new ArgumentException("Explicit --commissioning-check required.");
            var common = new List<string> { "--diagnostic" };
            string? version = null, expectedPath = null;
            var execute = false;
            for (var i = 1; i < args.Count; i++)
            {
                switch (args[i])
                {
                    case "--protocol-version":
                        if (version is not null || ++i == args.Count) throw new ArgumentException("One protocol version required.");
                        version = args[i]; break;
                    case "--expected-b0":
                        if (expectedPath is not null || ++i == args.Count) throw new ArgumentException("One reviewed B0 file required.");
                        expectedPath = args[i]; break;
                    case "--execute":
                        if (execute) throw new ArgumentException("Duplicate execution switch.");
                        execute = true; common.Add("--read-once"); break;
                    case "--read-once": throw new ArgumentException("Use --execute for the commissioning procedure.");
                    default: common.Add(args[i]); break;
                }
            }
            if (!ushort.TryParse(version, NumberStyles.None, CultureInfo.InvariantCulture, out protocol) || expectedPath is null)
                throw new ArgumentException("Explicit protocol and reviewed --expected-b0 required.");
            expected = JsonSerializer.Deserialize<ushort[]>(File.ReadAllText(expectedPath)) ?? [];
            if (expected.Length != 21 || expected[6] != protocol || expected[20] != 0)
                throw new ArgumentException("Invalid reviewed B0 image.");
            options = DiagnosticConsoleApplication.Parse(common);
            if (options.Settings is { } settings &&
                (settings.PortName != "COM7" || settings.BaudRate != 115200 || settings.Parity != ModbusSerialParity.Even))
                throw new ArgumentException("M014-G uses COM7/115200/8E1 exclusively.");
        }
        catch (Exception ex) when (ex is ArgumentException or IOException or JsonException or UnauthorizedAccessException)
        {
            await error.WriteLineAsync("COMMISSIONING usage: " + ex.Message);
            return SupervisionConsoleApplication.UsageOrConfigurationExitCode;
        }
        await output.WriteLineAsync("M014-G plan: B0/21; B1/20 x3 at 2s; wait100s; one planned timeout; close/invalidate; wait100s; fresh B0; B1 x3. Maximum9 FC03, retries0, no writes. OFFLINE default.");
        if (!options.ReadOnce)
        {
            await output.WriteLineAsync("OFFLINE: no port opened, no transmission, no journal/database created.");
            return 0;
        }

        FileStream? log = null;
        SupervisionRuntimeComposition? composition = null;
        var stage = "journal";
        var requests = 0;
        wait ??= (duration, cancellation) => Task.Delay(duration, cancellation);
        try
        {
            token.ThrowIfCancellationRequested();
            log = new FileStream(options.LogPath!, FileMode.CreateNew, FileAccess.Write, FileShare.None);
            await Record(new { @event = "start", utc = DateTimeOffset.UtcNow, maximumRequests = MaximumRequests,
                retries = 0, options.Settings, expectedB0 = expected, supportedProtocol = protocol,
                evidence = "decoded registers; target silence window is external physical precondition; no wire capture" });
            var bus = new SerialBus(BusId);
            var endpoint = new TR2Endpoint(bus, new ModbusAddress(1));
            var settings = options.Settings!;
            var configured = new RuntimeBusConfiguration(bus, [endpoint])
            {
                Serial = new RuntimeSerialPortConfiguration("COM7", 115200, 8,
                    RuntimeSerialParity.Even, RuntimeSerialStopBits.One,
                    TimeSpan.FromMilliseconds(settings.ResponseTimeoutMilliseconds))
            };
            var configuration = new RuntimeConfiguration(
                new SqlitePersistenceOptions(Path.ChangeExtension(options.LogPath!, ".sqlite")),
                [configured], RuntimePollingPolicy.Default, null, RuntimeWebConfiguration.Default);
            composition = SupervisionRuntimeCompositionRoot.Compose(configuration, factory);
            var recovery = new SerialBusRecoveryCoordinator(composition);
            stage = "open";
            await composition.BusConnectionManager.OpenAsync(BusId, settings, token);
            var deviceId = B0Identification.Parse(expected).DeviceId;
            await Identify("initial");
            await Poll("initial");
            var before = composition.TelemetrySnapshotRegistry.Get(deviceId).SystemState;
            stage = "wait-loss";
            await Record(new { @event = "wait", utc = DateTimeOffset.UtcNow, stage, milliseconds = LossDelay.TotalMilliseconds });
            await wait(LossDelay, token);
            stage = "planned-loss";
            try
            {
                await Read(1000, 20, stage);
                throw new InvalidDataException("Planned physical timeout did not occur; stop without repetition.");
            }
            catch (Exception ex) when (ex is TimeoutException || ex is ModbusTransportFailureException { Kind: ModbusTransportFailureKind.Timeout })
            {
                await Record(new { @event = "physical_timeout", utc = DateTimeOffset.UtcNow, type = ex.GetType().FullName, ex.Message });
            }
            stage = "disconnect";
            await recovery.MarkDisconnectedAsync(bus, token);
            var unavailable = composition.TelemetrySnapshotRegistry.Get(deviceId).SystemState;
            if (unavailable.IsAvailable || unavailable.ReceivedAt != before.ReceivedAt || unavailable.LastValue != before.LastValue ||
                composition.FleetRegistry.GetSession(endpoint).State != TR2SessionState.Unidentified ||
                composition.BusConnectionManager.TryGet(BusId, out _))
                throw new InvalidDataException("Disconnect did not preserve/invalidate last value and forget identity/connection.");
            await Record(new { @event = "unavailable", utc = DateTimeOffset.UtcNow, unavailable });
            stage = "wait-recovery";
            await Record(new { @event = "wait", utc = DateTimeOffset.UtcNow, stage, milliseconds = RecoveryDelay.TotalMilliseconds });
            await wait(RecoveryDelay, token);
            stage = "reopen";
            if (!await recovery.TryReconnectAsync(bus, token)) throw new IOException("Explicit reopen failed.");
            if (composition.FleetRegistry.GetSession(endpoint).State != TR2SessionState.Unidentified)
                throw new InvalidDataException("Reopen reused an old identity.");
            await Identify("recovery");
            if (composition.TelemetrySnapshotRegistry.Get(deviceId).SystemState.IsAvailable)
                throw new InvalidDataException("B0 alone reactivated stale B1.");
            await Poll("recovery");
            var fresh = composition.TelemetrySnapshotRegistry.Get(deviceId).SystemState;
            if (!fresh.IsAvailable || fresh.ReceivedAt <= before.ReceivedAt)
                throw new InvalidDataException("Fresh B1 not received after reconnect.");
            stage = "final-close";
            await composition.BusConnectionManager.CloseAsync(BusId);
            await Record(new { @event = "result", utc = DateTimeOffset.UtcNow, passed = true,
                requests, initial = before, afterLoss = unavailable, recovered = fresh,
                note = "Physical availability loss/recovery only; not cable removal, READY, vibration or SD write qualification" });
            await output.WriteLineAsync($"M014-G completed: DeviceId={deviceId.Value}; requests={requests}; last value preserved then fresh B1; two serial sessions closed; no retry.");
            return 0;

            async Task Identify(string phase)
            {
                stage = phase + "-B0";
                var registers = await Read(0, 21, stage);
                if (!registers.SequenceEqual(expected)) throw new InvalidDataException("B0 differs from reviewed identity.");
                var session = B0SessionFactory.Create(endpoint, registers, protocol);
                if (session.State != TR2SessionState.Compatible || session.Device is null)
                    throw new InvalidDataException("B0 incompatible; no B1 permitted.");
                composition.FleetRegistry.SetSession(session);
                await Record(new { @event = "identity", utc = DateTimeOffset.UtcNow, phase, deviceId = session.Device.DeviceId.Value });
            }
            async Task Poll(string phase)
            {
                for (var i = 0; i < PollsPerPhase; i++)
                {
                    stage = phase + "-B1-" + i;
                    var registers = await Read(1000, 20, stage);
                    var value = B1SystemState.Parse(registers);
                    if (registers.Skip(17).Any(r => r != 0)) throw new InvalidDataException("Reserved B1 registers nonzero.");
                    var received = DateTimeOffset.UtcNow;
                    var snapshots = composition.TelemetrySnapshotRegistry.ReceiveSystemState(deviceId, value, received);
                    await Record(new { @event = "telemetry", utc = received, phase, index = i, snapshot = snapshots.SystemState });
                    if (i + 1 < PollsPerPhase) await wait(Interval, token);
                }
            }
            async Task<ushort[]> Read(ushort address, ushort count, string phase)
            {
                token.ThrowIfCancellationRequested();
                if (requests >= MaximumRequests) throw new InvalidOperationException("Request budget exhausted.");
                if (!composition.BusConnectionManager.TryGet(BusId, out var connection) || connection is null)
                    throw new IOException("No current serial connection.");
                await Record(new { @event = "read_intent", utc = DateTimeOffset.UtcNow, phase, function = 3, unit = 1, address, count });
                requests++;
                var pending = connection.RegisterTransport.ReadRegistersAsync(BusId, 1, address, count, token).AsTask();
                _ = pending.ContinueWith(t => { _ = t.Exception; }, CancellationToken.None,
                    TaskContinuationOptions.OnlyOnFaulted | TaskContinuationOptions.ExecuteSynchronously, TaskScheduler.Default);
                var registers = await pending.WaitAsync(TimeSpan.FromMilliseconds(settings.ResponseTimeoutMilliseconds * 4), token);
                await Record(new { @event = "decoded_response", utc = DateTimeOffset.UtcNow, phase, address, registers });
                if (registers.Length != count) throw new InvalidDataException("Wrong response register count.");
                return registers;
            }
            async Task Record(object value)
            {
                await log.WriteAsync(Encoding.UTF8.GetBytes(JsonSerializer.Serialize(value) + "\n"), token);
                log.Flush(flushToDisk: true);
            }
        }
        catch (Exception ex)
        {
            var failure = new { @event = "failure", utc = DateTimeOffset.UtcNow, stage, requests,
                type = ex.GetType().FullName, ex.Message, passed = false, noRetry = true };
            if (log is not null)
            {
                try { await log.WriteAsync(Encoding.UTF8.GetBytes(JsonSerializer.Serialize(failure) + "\n")); log.Flush(true); }
                catch (Exception journalError) { await error.WriteLineAsync("Journal failure: " + journalError.Message); }
            }
            await error.WriteLineAsync(JsonSerializer.Serialize(failure));
            return 1;
        }
        finally
        {
            if (composition is not null) await composition.BusConnectionManager.DisposeAsync();
            if (log is not null) await log.DisposeAsync();
        }
    }
}
