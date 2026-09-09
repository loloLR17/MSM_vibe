namespace TR2.Protocol;

public sealed record B5CommandState(
    ushort RequestCode,
    ushort RequestTransactionId,
    ushort RequestParam1,
    ushort RequestParam2,
    uint RequestParam3,
    ushort RequestConfirmKey,
    ushort RequestControl,
    ushort ActiveCode,
    ushort ActiveTransactionId,
    ushort Status,
    ushort ResultCode,
    ushort ResultDetail,
    ushort EngineFlags,
    ushort LastCode,
    ushort LastTransactionId,
    ushort LastStatusFinal,
    ushort LastResultCode,
    uint LastTimestampSeconds)
{
    public const ushort StartAddress = 5000;
    public const ushort RegisterCount = 20;

    public static B5CommandState Parse(ReadOnlySpan<ushort> registers)
    {
        if (registers.Length < RegisterCount)
        {
            throw new ArgumentException(
                $"B5 requires at least {RegisterCount} registers.",
                nameof(registers));
        }

        return new B5CommandState(
            registers[0],
            registers[1],
            registers[2],
            registers[3],
            UInt32(registers[4], registers[5]),
            registers[6],
            registers[7],
            registers[8],
            registers[9],
            registers[10],
            registers[11],
            registers[12],
            registers[13],
            registers[14],
            registers[15],
            registers[16],
            registers[17],
            UInt32(registers[18], registers[19]));
    }

    private static uint UInt32(ushort msw, ushort lsw) =>
        ((uint)msw << 16) | lsw;
}
