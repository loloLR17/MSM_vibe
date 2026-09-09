namespace TR2.Protocol;

public sealed record B1SystemState(
    ushort SystemStatus,
    ushort SystemFlags,
    ushort FaultFlags,
    ushort WarningFlags,
    uint UptimeSeconds,
    ushort LastResetCause,
    short InternalTemperatureDeciCelsius,
    ushort CpuLoadPercent,
    ushort MemoryUsagePercent,
    ushort StorageStatus,
    ushort StorageUsagePercent,
    ushort AcquisitionState,
    uint ActiveCampaignId,
    ushort ErrorCode,
    ushort WarningCode)
{
    public const ushort StartAddress = 1000;
    public const int RegisterCount = 20;

    public static B1SystemState Parse(ReadOnlySpan<ushort> registers)
    {
        if (registers.Length < RegisterCount)
        {
            throw new ArgumentException(
                $"B1 requires at least {RegisterCount} registers.",
                nameof(registers));
        }

        return new B1SystemState(
            registers[0],
            registers[1],
            registers[2],
            registers[3],
            UInt32(registers[4], registers[5]),
            registers[6],
            unchecked((short)registers[7]),
            registers[8],
            registers[9],
            registers[10],
            registers[11],
            registers[12],
            UInt32(registers[13], registers[14]),
            registers[15],
            registers[16]);
    }

    private static uint UInt32(ushort msw, ushort lsw) =>
        ((uint)msw << 16) | lsw;
}
