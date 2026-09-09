using TR2.Transport;

namespace TR2.Supervision.Service;

internal static class RuntimeSerialTransportMapper
{
    public static ModbusSerialConnectionSettings Map(RuntimeSerialPortConfiguration configuration)
    {
        ArgumentNullException.ThrowIfNull(configuration);

        return new ModbusSerialConnectionSettings(
            configuration.PortName,
            configuration.BaudRate,
            configuration.DataBits,
            MapParity(configuration.Parity),
            MapStopBits(configuration.StopBits),
            checked((int)configuration.ResponseTimeout.TotalMilliseconds));
    }

    private static ModbusSerialParity MapParity(RuntimeSerialParity parity) => parity switch
    {
        RuntimeSerialParity.None => ModbusSerialParity.None,
        RuntimeSerialParity.Odd => ModbusSerialParity.Odd,
        RuntimeSerialParity.Even => ModbusSerialParity.Even,
        RuntimeSerialParity.Mark => ModbusSerialParity.Mark,
        RuntimeSerialParity.Space => ModbusSerialParity.Space,
        _ => throw new ArgumentOutOfRangeException(nameof(parity))
    };

    private static ModbusSerialStopBits MapStopBits(RuntimeSerialStopBits stopBits) => stopBits switch
    {
        RuntimeSerialStopBits.One => ModbusSerialStopBits.One,
        RuntimeSerialStopBits.Two => ModbusSerialStopBits.Two,
        RuntimeSerialStopBits.OnePointFive => ModbusSerialStopBits.OnePointFive,
        _ => throw new ArgumentOutOfRangeException(nameof(stopBits))
    };
}
