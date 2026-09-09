namespace TR2.Protocol;

public sealed record B5CommandRequest(
    ushort Code,
    ushort TransactionId,
    ushort Param1,
    ushort Param2,
    uint Param3,
    ushort ConfirmKey)
{
    public const ushort StartAddress = 5000;
    public const ushort ControlAddress = 5007;
    public const ushort RequestRegisterCount = 8;
    public const ushort SubmitControlValue = 0x0001;

    public ushort[] ToPreparedRegisters() =>
    [
        Code,
        TransactionId,
        Param1,
        Param2,
        (ushort)(Param3 >> 16),
        (ushort)(Param3 & 0xFFFF),
        ConfirmKey,
        0
    ];
}
