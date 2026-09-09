using NModbus;

namespace TR2.Transport;

internal sealed class NModbusRegisterClient : IModbusRegisterClient
{
    private readonly IModbusMaster _master;

    public NModbusRegisterClient(IModbusMaster master)
    {
        ArgumentNullException.ThrowIfNull(master);

        NModbusSafetyPolicy.Apply(master);
        _master = master;
    }

    public Task<ushort[]> ReadHoldingRegistersAsync(
        byte unitAddress,
        ushort startAddress,
        ushort registerCount) =>
        _master.ReadHoldingRegistersAsync(unitAddress, startAddress, registerCount);

    public Task WriteMultipleRegistersAsync(
        byte unitAddress,
        ushort startAddress,
        ushort[] values) =>
        _master.WriteMultipleRegistersAsync(unitAddress, startAddress, values);
}
