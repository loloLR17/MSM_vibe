namespace TR2.Protocol;

// Experimental operator-only AC1 v1, explicitly authorized 2026-10-28.
// Not an allocation in LIVE R0, and never a reinterpretation of B3.
public sealed record LiveAc1Snapshot(ushort Flags, ushort FullScaleCode, ushort VerifiedCtrl1Xl,
    uint SamplingFrequencyHz, uint CutoffMillihertz, uint WarmupSamples, ulong RunId,
    uint ConfigurationId, uint ConfigurationGeneration, uint Sequence, uint SampleCount,
    uint WindowDurationMilliseconds, uint ValueAgeMilliseconds,
    int DcXMicroG, int DcYMicroG, int DcZMicroG,
    uint RmsGlobalMg, uint RmsXMg, uint RmsYMg, uint RmsZMg,
    uint PeakGlobalMg, uint PeakXMg, uint PeakYMg, uint PeakZMg, uint FsReadbackSequence, ushort AxesEnableMask)
{
    public const ushort StartAddress = 8000, RegisterCount = 64;
    public string Extension => "AC1-v1 experimental NON-PRODUCTION";
    public string Method => "Continuous per-axis complementary high-pass, one-pole DC estimate, 2 Hz";
    public string Unit => "mg";
    public bool ValuesValid => (Flags & 1) != 0;
    public bool Running => (Flags & 2) != 0;
    public bool Fresh => (Flags & 4) != 0;
    public bool Held => (Flags & 8) != 0;
    public bool FullScaleProved => (Flags & 16) != 0;
    public bool WarmingUp => (Flags & 32) != 0;
    public static LiveAc1Snapshot Parse(ReadOnlySpan<ushort> r)
    {
        if (r.Length != 64 || r[0] != 0x4143 || r[1] != 1 || r[3] != 1 || r[4] > 3 || (r[2] & ~63) != 0)
            throw new InvalidDataException("Unsupported experimental AC1 header/flags.");
        for (var i = 53; i < 64; i++) if (r[i] != 0) throw new InvalidDataException("Nonzero AC1 reserved word.");
        var s = new LiveAc1Snapshot(r[2], r[4], r[5], U32(r, 6), U32(r, 8), U32(r, 10),
            ((ulong)U32(r, 12) << 32) | U32(r, 14), U32(r, 16), U32(r, 18), U32(r, 20), U32(r, 22),
            U32(r, 24), U32(r, 26), unchecked((int)U32(r, 28)), unchecked((int)U32(r, 30)), unchecked((int)U32(r, 32)),
            U32(r, 34), U32(r, 36), U32(r, 38), U32(r, 40), U32(r, 42), U32(r, 44), U32(r, 46), U32(r, 48), U32(r, 50), r[52]);
        if (s.AxesEnableMask is < 1 or > 7 || s.SamplingFrequencyHz != 26667 || s.CutoffMillihertz != 2000 || s.WarmupSamples != 26667 ||
            (s.Fresh && (!s.ValuesValid || !s.Running || s.Held)) || (s.Held && (!s.ValuesValid || s.Running)) ||
            (s.WarmingUp && s.ValuesValid) || (s.ValuesValid && (s.RunId == 0 || s.ConfigurationId == 0 || s.ConfigurationGeneration == 0 || s.Sequence == 0 || s.SampleCount is not (4096 or 8192 or 16384 or 32768))) ||
            (s.FullScaleProved && (s.VerifiedCtrl1Xl != new ushort[] { 0xA0, 0xA8, 0xAC, 0xA4 }[s.FullScaleCode] || s.FsReadbackSequence == 0)) ||
            s.RmsGlobalMg > s.PeakGlobalMg || s.PeakGlobalMg > 60000)
            throw new InvalidDataException("Incoherent AC1 metadata/quality/physical FS evidence.");
        return s;
    }
    private static uint U32(ReadOnlySpan<ushort> r, int i) => ((uint)r[i] << 16) | r[i + 1];
}
