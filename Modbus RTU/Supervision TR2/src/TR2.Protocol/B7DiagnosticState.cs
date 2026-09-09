namespace TR2.Protocol;

public sealed record B7DiagnosticState(
    ushort DiagnosticStructureVersion,
    ushort SystemHealthStatus,
    ushort SystemFaultFlags,
    ushort LastFaultCode,
    uint LastFaultTimestampSeconds,
    ushort SelftestStatus,
    ushort SelftestResultCode,
    ushort SelftestDetail,
    uint UptimeSeconds,
    ushort ResetCause,
    short InternalTemperatureDeciCelsius,
    ushort SupplyVoltageMillivolts)
{
    public const ushort StartAddress = 7000;
    public const ushort RegisterCount = 16;

    public static B7DiagnosticState Parse(ReadOnlySpan<ushort> registers)
    {
        if (registers.Length < RegisterCount)
        {
            throw new ArgumentException(
                $"B7 requires at least {RegisterCount} registers.",
                nameof(registers));
        }

        return new B7DiagnosticState(
            registers[0],
            registers[1],
            registers[2],
            registers[3],
            UInt32(registers[4], registers[5]),
            registers[6],
            registers[7],
            registers[8],
            UInt32(registers[9], registers[10]),
            registers[11],
            unchecked((short)registers[12]),
            registers[13]);
    }

    private static uint UInt32(ushort msw, ushort lsw) =>
        ((uint)msw << 16) | lsw;
}
