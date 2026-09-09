namespace TR2.Protocol;

public sealed record B2TimeState(
    ushort TimeStatus,
    ushort TimeFlags,
    uint CurrentTimeSeconds,
    uint LastSyncTimeSeconds,
    uint TimeSinceSyncSeconds,
    uint PreparedTimeSeconds,
    ushort PreparedTimeStatus,
    ushort TimeAccuracyMilliseconds,
    short DriftPpm,
    ushort SyncSource)
{
    public const ushort StartAddress = 2000;
    public const ushort RegisterCount = 16;

    public static B2TimeState Parse(ReadOnlySpan<ushort> registers)
    {
        if (registers.Length < RegisterCount)
        {
            throw new ArgumentException(
                $"B2 requires at least {RegisterCount} registers.",
                nameof(registers));
        }

        return new B2TimeState(
            registers[0],
            registers[1],
            UInt32(registers[2], registers[3]),
            UInt32(registers[4], registers[5]),
            UInt32(registers[6], registers[7]),
            UInt32(registers[8], registers[9]),
            registers[10],
            registers[11],
            unchecked((short)registers[12]),
            registers[13]);
    }

    private static uint UInt32(ushort msw, ushort lsw) =>
        ((uint)msw << 16) | lsw;
}
