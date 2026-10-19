using System.Diagnostics;
using System.Text.Json;
using TR2.Transport;

namespace TR2.Supervision.Service;

// Explicit operator tool for the partial R2 target; the observation host remains read-only.
// Pauses its sole poller and waits for the tokenized actual-handle-release acknowledgment.
public static class LiveOperatorConsoleApplication
{
    public sealed record Ledger(uint Device, uint Epoch, ushort Transaction);
    public static (uint Epoch, ushort Transaction) Allocate(ushort[] b5, Ledger? previous)
    {
        if (b5.Length != 29 || (b5[13] & 2) != 0 || b5[10] == 9 || b5[28] != 1)
            throw new InvalidDataException("B5 actif/indéterminé ou epoch non valide ; aucune écriture.");
        var epoch = ((uint)b5[22] << 16) | b5[23];
        if (epoch == 0 || (previous is not null && previous.Device != 1))
            throw new InvalidDataException("Identité/epoch de garde invalide.");
        var lastEpoch = ((uint)b5[26] << 16) | b5[27];
        var used = Math.Max(13, lastEpoch == epoch ? b5[15] : 0); // Never reuse historical 5/12,5/13.
        if (previous?.Epoch == epoch) used = Math.Max(used, previous.Transaction);
        if (previous is not null && previous.Epoch > epoch) throw new InvalidDataException("Recul d'epoch interdit.");
        if (used >= 65534) throw new InvalidDataException("Identifiants épuisés ; aucun renouvellement automatique.");
        return (epoch, checked((ushort)(used + 1)));
    }
    public static bool IsTerminal(ushort[] b5, uint epoch, ushort transaction, ushort code) =>
        b5.Length == 29 && ((uint)b5[26] << 16 | b5[27]) == epoch && b5[14] == code && b5[15] == transaction &&
        b5[16] == 4 && b5[17] == 0 && (b5[13] & 2) == 0 && b5[10] == 4;

    public static async Task<int> RunAsync(IReadOnlyList<string> args, CancellationToken ct, TextWriter output, TextWriter error, IModbusBusConnectionFactory? factory = null)
    {
        if (args.Count != 8 || args[0] != "--live-command" || args[1] is not ("start" or "stop") ||
            args[2] != "--port" || args[3] != "COM4" || args[4] != "--control" || args[6] != "--state")
        { await error.WriteLineAsync("Usage: --live-command start|stop --port COM4 --control <service-control> --state <persistent-ledger.json>. Diagnostic TR2-001 only; service must be running."); return 2; }
        var control = args[5]; var token = "disconnected:" + Guid.NewGuid(); var ownsPause = false;
        void Log(object value) { output.WriteLine(JsonSerializer.Serialize(value)); output.Flush(); }
        try
        {
            using var exclusiveOperator = new FileStream(control + ".command.lock", FileMode.OpenOrCreate, FileAccess.ReadWrite, FileShare.None);
            using (var pause = new FileStream(control, FileMode.CreateNew, FileAccess.Write, FileShare.Read))
            { using var writer = new StreamWriter(pause); writer.Write(token); writer.Flush(); pause.Flush(true); }
            ownsPause = true;
            var waited = Stopwatch.StartNew();
            while (LiveDirectConsoleApplication.ReadControlFile(control + ".released") != token)
            { if (waited.Elapsed > TimeSpan.FromSeconds(15)) throw new TimeoutException("Service non prêt ou handle non libéré ; aucune transmission."); await Task.Delay(100, ct); }
            Log(new { state = "poller_paused_handle_released", utc = DateTimeOffset.UtcNow });
            await using var connection = await (factory ?? new NModbusSerialBusConnectionFactory(row => Log(row))).OpenAsync("operator", new("COM4", 115200, 8, ModbusSerialParity.Even, ModbusSerialStopBits.One, 1500), ct);
            async ValueTask<ushort[]> Read(ushort address, ushort count) => await connection.RegisterTransport.ReadRegistersAsync("operator", 1, address, count, ct);
            var b0 = await Read(0, 21);
            if (b0[0] != 0 || b0[1] != 1 || b0[6] != 2 || b0[7] != 12) throw new InvalidDataException("Profil diagnostique TR2-001 attendu ; aucun write.");
            var initial = await Read(3000, 48); var b5 = await Read(5000, 29);
            if (args[1] == "start" ? initial[0] != 1 || (initial[1] & 6) != 0 : (initial[1] & 6) != 6)
                throw new InvalidDataException("État B3 incompatible avec la commande ; aucune écriture.");
            Ledger? previous = File.Exists(args[7]) ? JsonSerializer.Deserialize<Ledger>(File.ReadAllText(args[7])) ?? throw new InvalidDataException("Ledger nul.") : null;
            var (epoch, tx) = Allocate(b5, previous);
            // Persist the identity BEFORE submit. A failed/uncertain request is never silently reused.
            var temporary = args[7] + "." + Guid.NewGuid() + ".tmp";
            using (var f = new FileStream(temporary, FileMode.CreateNew, FileAccess.Write, FileShare.None))
            { JsonSerializer.Serialize(f, new Ledger(1, epoch, tx)); f.Flush(true); }
            File.Move(temporary, args[7], true);
            var code = (ushort)(args[1] == "start" ? 13 : 14); var watch = Stopwatch.StartNew();
            Log(new { state = "requested_not_terminal", command = args[1], epoch, transaction = tx, utc = DateTimeOffset.UtcNow, message = "Demande en cours ; l'acceptation ne prouve pas l'effet. STOP peut prendre 30–60 s." });
            await connection.RegisterWriteTransport.WriteRegistersAsync("operator", 1, 5020, [(ushort)(epoch >> 16), (ushort)epoch], ct);
            await connection.RegisterWriteTransport.WriteRegistersAsync("operator", 1, 5000, [code, tx, 0, 0, 0, 0, 0, 1], ct);
            while (watch.Elapsed < TimeSpan.FromSeconds(210))
            {
                b5 = await Read(5000, 29); var b3 = await Read(3000, 48);
                Log(new { state = "in_progress", command = args[1], epoch, transaction = tx, b5, b3, utc = DateTimeOffset.UtcNow });
                if (IsTerminal(b5, epoch, tx, code))
                {
                    var effect = code == 13 ? (b3[1] & 6) == 6 : b3[0] == 1 && (b3[1] & 4) == 0 && (b3[1] & 512) != 0;
                    if (!effect) throw new InvalidDataException("Terminal observé mais effet B3 attendu absent ; diagnostic requis.");
                    Log(new { state = "terminal_and_effect_observed", command = args[1], epoch, transaction = tx, seconds = watch.Elapsed.TotalSeconds, b5, b3, utc = DateTimeOffset.UtcNow });
                    return 0;
                }
                if (b5[10] is 5 or 6 or 7 or 8 or 9) throw new InvalidDataException("Commande refusée/échouée/indéterminée ; aucun retry automatique.");
                await Task.Delay(250, ct);
            }
            throw new TimeoutException("Terminal non observé ; état incertain, pas de répétition automatique. Contrôle STOP/diagnostic requis.");
        }
        catch (Exception ex) when (ex is not OutOfMemoryException)
        { await error.WriteLineAsync(JsonSerializer.Serialize(new { state = "failed_or_uncertain", error = ex.Message, utc = DateTimeOffset.UtcNow })); return 1; }
        finally
        {
            // Connection disposal precedes return/finally, so observation resumes only after writer closes.
            if (ownsPause && LiveDirectConsoleApplication.ReadControlFile(control) == token) File.Delete(control);
        }
    }
}
