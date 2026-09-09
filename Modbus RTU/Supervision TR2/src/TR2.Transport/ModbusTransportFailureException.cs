namespace TR2.Transport;

public enum ModbusTransportFailureKind
{
    Timeout,
    Io
}

public sealed class ModbusTransportFailureException : Exception
{
    public ModbusTransportFailureException(
        ModbusTransportFailureKind kind,
        string message,
        Exception innerException)
        : base(message, innerException)
    {
        Kind = kind;
    }

    public ModbusTransportFailureKind Kind { get; }
}
