namespace TR2.Protocol;

public sealed record B3VibrationSupervision(
    ushort StatusGlobal,
    ushort ValidityFlags,
    ushort AlarmFlags,
    ushort SeverityGlobal,
    uint LastUpdateTr2Seconds,
    uint ValueAgeMilliseconds,
    uint CalculationSequence,
    uint WindowDurationMilliseconds,
    uint ValidSampleCount,
    uint RmsGlobalMg,
    uint PeakGlobalMg,
    uint RmsXMg,
    uint RmsYMg,
    uint RmsZMg,
    uint PeakXMg,
    uint PeakYMg,
    uint PeakZMg,
    ushort DominantAxis,
    ushort ExceedGlobal,
    ushort ExceedX,
    ushort ExceedY,
    ushort ExceedZ,
    ushort AlarmLatched,
    uint ExceedCount,
    uint AlarmCount)
{
    public const ushort StartAddress = 3000;
    public const ushort RegisterCount = 48;

    public static B3VibrationSupervision Parse(ReadOnlySpan<ushort> registers)
    {
        if (registers.Length < RegisterCount)
        {
            throw new ArgumentException(
                $"B3 requires at least {RegisterCount} registers.",
                nameof(registers));
        }

        return new B3VibrationSupervision(
            registers[0],
            registers[1],
            registers[2],
            registers[3],
            UInt32(registers[4], registers[5]),
            UInt32(registers[6], registers[7]),
            UInt32(registers[8], registers[9]),
            UInt32(registers[10], registers[11]),
            UInt32(registers[12], registers[13]),
            UInt32(registers[14], registers[15]),
            UInt32(registers[16], registers[17]),
            UInt32(registers[18], registers[19]),
            UInt32(registers[20], registers[21]),
            UInt32(registers[22], registers[23]),
            UInt32(registers[24], registers[25]),
            UInt32(registers[26], registers[27]),
            UInt32(registers[28], registers[29]),
            registers[30],
            registers[31],
            registers[32],
            registers[33],
            registers[34],
            registers[35],
            UInt32(registers[36], registers[37]),
            UInt32(registers[38], registers[39]));
    }

    private static uint UInt32(ushort msw, ushort lsw) =>
        ((uint)msw << 16) | lsw;
}
