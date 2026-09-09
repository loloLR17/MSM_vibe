namespace TR2.Transport;

public enum ModbusTransportFailureKind
{
    Timeout,
    Io,
    ModbusExceptionResponse
}

public sealed class ModbusTransportFailureException : Exception
{
    public ModbusTransportFailureException(
        ModbusTransportFailureKind kind,
        string message,
        Exception innerException,
        byte? functionCode = null,
        byte? exceptionCode = null)
        : base(message, innerException)
    {
        Kind = kind;
        FunctionCode = functionCode;
        ExceptionCode = exceptionCode;
    }

    public ModbusTransportFailureKind Kind { get; }
    public byte? FunctionCode { get; }
    public byte? ExceptionCode { get; }
}
