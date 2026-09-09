namespace TR2.Transport;

public interface IModbusRegisterClient
{
    Task<ushort[]> ReadHoldingRegistersAsync(
        byte unitAddress,
        ushort startAddress,
        ushort registerCount);

    Task WriteMultipleRegistersAsync(
        byte unitAddress,
        ushort startAddress,
        ushort[] values);
}
