namespace TR2.Transport;

public enum ModbusSerialParity
{
    None,
    Odd,
    Even,
    Mark,
    Space
}

public enum ModbusSerialStopBits
{
    One,
    Two,
    OnePointFive
}

public sealed record ModbusSerialConnectionSettings
{
    public ModbusSerialConnectionSettings(
        string portName,
        int baudRate,
        int dataBits,
        ModbusSerialParity parity,
        ModbusSerialStopBits stopBits,
        int responseTimeoutMilliseconds)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(portName);

        if (baudRate <= 0)
        {
            throw new ArgumentOutOfRangeException(nameof(baudRate));
        }

        if (dataBits <= 0)
        {
            throw new ArgumentOutOfRangeException(nameof(dataBits));
        }

        if (responseTimeoutMilliseconds <= 0)
        {
            throw new ArgumentOutOfRangeException(nameof(responseTimeoutMilliseconds));
        }

        PortName = portName;
        BaudRate = baudRate;
        DataBits = dataBits;
        Parity = parity;
        StopBits = stopBits;
        ResponseTimeoutMilliseconds = responseTimeoutMilliseconds;
    }

    public string PortName { get; }
    public int BaudRate { get; }
    public int DataBits { get; }
    public ModbusSerialParity Parity { get; }
    public ModbusSerialStopBits StopBits { get; }
    public int ResponseTimeoutMilliseconds { get; }
}
