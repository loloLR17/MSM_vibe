using TR2.Domain;
using TR2.Transport;

namespace TR2.Protocol;

public enum SupervisionProfile { LegacyV1, Live, LiveDiagnostic }

public sealed record LiveProfileIdentity(DeviceId DeviceId, SupervisionProfile Profile,
    ushort ProtocolVersion, ushort Capabilities, ushort? ConfigurationStructureVersion,
    ushort? InventoryStructureVersion, ushort? DiagnosticStructureVersion)
{
    public ushort HardwareVersion { get; init; }
    public ushort FirmwareMajor { get; init; }
    public ushort FirmwareMinor { get; init; }
    public ushort FirmwarePatch { get; init; }
}

// Uses only the read boundary. No command writer, epoch lookup or automatic B4/B5 write.
public sealed class LiveProfileReader(IRegisterTransport transport, bool allowDiagnostic = false)
{
    private async ValueTask<ushort[]> Read(TR2Endpoint endpoint, ushort address, ushort count, CancellationToken ct)
    {
        var words = await transport.ReadRegistersAsync(endpoint.Bus.Id, endpoint.Address.Value, address, count, ct);
        if (words.Length != count) throw new InvalidDataException($"Expected {count} registers at {address}, got {words.Length}.");
        return words;
    }

    public async ValueTask<LiveProfileIdentity> IdentifyAsync(TR2Endpoint endpoint, CancellationToken ct = default)
    {
        var b0 = await Read(endpoint, 0, 21, ct);
        var id = B0Identification.Parse(b0);
        if (id.ProtocolVersion == 1)
            return new(id.DeviceId, SupervisionProfile.LegacyV1, 1, b0[7], null, null, null)
            { HardwareVersion = b0[2], FirmwareMajor = b0[3], FirmwareMinor = b0[4], FirmwarePatch = b0[5] };
        if (id.ProtocolVersion != 2 || b0[20] != 0 || (b0[7] & ~0x1F) != 0)
            throw new InvalidDataException("Unknown protocol or reserved B0 fields.");
        var product = (b0[7] & 16) != 0;
        if (product && (b0[7] & 0x1D) != 0x1D)
            throw new InvalidDataException("LIVE capability conflicts with required acquisition/supervision/diagnostics.");
        if (!product && !allowDiagnostic)
            throw new NotSupportedException("Partial protocol 2 target: explicit readonly diagnostic opt-in required.");
        var b4 = await Read(endpoint, 4000, 2, ct);
        var b6 = await Read(endpoint, 6000, 1, ct);
        if (b4[0] != 2 || (b4[1] & 1) == 0 || (b4[1] & ~3) != 0 || b6[0] != 2)
            throw new InvalidDataException("Unsupported LIVE configuration/inventory structures.");
        ushort? b7 = null;
        if (product)
        {
            b7 = (await Read(endpoint, 7000, 1, ct))[0];
            if (b7 != 1) throw new InvalidDataException("Unsupported diagnostic structure.");
        }
        return new(id.DeviceId, product ? SupervisionProfile.Live : SupervisionProfile.LiveDiagnostic,
            2, b0[7], b4[0], b6[0], b7)
        { HardwareVersion = b0[2], FirmwareMajor = b0[3], FirmwareMinor = b0[4], FirmwarePatch = b0[5] };
    }

    public async ValueTask<LiveB3Snapshot> ReadAsync(TR2Endpoint endpoint, LiveProfileIdentity identity, CancellationToken ct = default)
    {
        if (identity.ProtocolVersion != 2 || identity.Profile == SupervisionProfile.LegacyV1)
            throw new NotSupportedException("Legacy V1 requires its existing decoder; it does not imply epoch support.");
        var words = await Read(endpoint, 3000, 48, ct);
        try { return LiveB3Snapshot.Parse(words); }
        catch (InvalidDataException ex) { throw new LiveB3ReadException(words, ex); }
    }
}

public sealed record LiveB3Snapshot(B3VibrationSupervision Value)
{
    public bool ValuesValid => (Value.ValidityFlags & 2) != 0;
    public bool Fresh => (Value.ValidityFlags & 4) != 0;
    public bool Held => (Value.ValidityFlags & 512) != 0;
    public bool AcquisitionStopped => (Value.ValidityFlags & 4096) != 0;
    public DateTimeOffset? WindowUtc => (Value.ValidityFlags & 64) != 0
        ? DateTimeOffset.FromUnixTimeSeconds(Value.LastUpdateTr2Seconds) : null;

    public static LiveB3Snapshot Parse(ReadOnlySpan<ushort> words)
    {
        if (words.Length != 48) throw new InvalidDataException("LIVE B3 must have exactly 48 registers.");
        for (var i = 40; i < 48; i++)
            if (words[i] != 0) throw new InvalidDataException("Nonzero reserved B3 register.");
        var v = B3VibrationSupervision.Parse(words);
        var f = v.ValidityFlags;
        bool Flag(int mask) => (f & mask) != 0;
        if (v.StatusGlobal > 5 || v.SeverityGlobal > 4 || (f & 0xE100) != 0 ||
            (v.AlarmFlags & ~0x1FF) != 0 || v.DominantAxis > 4 || v.ExceedGlobal > 1 ||
            v.ExceedX > 1 || v.ExceedY > 1 || v.ExceedZ > 1 || v.AlarmLatched > 1)
            throw new InvalidDataException("Reserved LIVE B3 enum/flags.");
        var expectedSeverity = !Flag(2) || !Flag(4) ? 0 : (v.AlarmFlags & 15) != 0 ? 4 :
            (v.AlarmFlags & 240) != 0 ? 3 : !Flag(64) ? 2 : 1;
        if (v.SeverityGlobal != expectedSeverity ||
            (v.StatusGlobal is 0 or 2 or 5 && Flag(2)) ||
            (Flag(2) && !Flag(4) && !Flag(512)) ||
            (Flag(4) && (!Flag(2) || Flag(512) || Flag(4096))) ||
            (Flag(2) && ((f & 0xB9) != 0xB9 || v.ValidSampleCount == 0 || v.CalculationSequence == 0 || Flag(2048))) ||
            (!Flag(64) && v.LastUpdateTr2Seconds != 0) ||
            ((!Flag(2) || !Flag(4)) && v.SeverityGlobal != 0) ||
            (v.StatusGlobal is 0 or 1 or 2 or 5 && Flag(4)) ||
            (v.StatusGlobal == 1 && !Flag(4096)) ||
            (v.StatusGlobal == 3 && (!Flag(2) || !Flag(4) || !Flag(64) || Flag(1024))))
            throw new InvalidDataException("Inconsistent LIVE B3 quality/UTC/state.");
        return new(v);
    }
}

public sealed class LiveB3ReadException : Exception
{
    public LiveB3ReadException(ushort[] words, InvalidDataException cause) : base(cause.Message, cause)
    {
        Registers = Array.AsReadOnly(words.ToArray());
        Decoded = B3VibrationSupervision.Parse(words);
    }
    public IReadOnlyList<ushort> Registers { get; }
    public B3VibrationSupervision Decoded { get; }
}
