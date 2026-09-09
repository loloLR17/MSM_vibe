using NModbus;

namespace TR2.Transport;

internal sealed class NModbusSerialBusConnection : IModbusBusConnection
{
    private readonly IModbusMaster _master;
    private bool _disposed;

    public NModbusSerialBusConnection(
        string busId,
        IModbusMaster master,
        ModbusRegisterTransport registerTransport)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(busId);
        ArgumentNullException.ThrowIfNull(master);
        ArgumentNullException.ThrowIfNull(registerTransport);

        BusId = busId;
        _master = master;
        RegisterTransport = registerTransport;
        RegisterWriteTransport = registerTransport;
    }

    public string BusId { get; }
    public IRegisterTransport RegisterTransport { get; }
    public IRegisterWriteTransport RegisterWriteTransport { get; }

    public ValueTask DisposeAsync()
    {
        if (_disposed)
        {
            return ValueTask.CompletedTask;
        }

        _disposed = true;
        _master.Dispose();
        return ValueTask.CompletedTask;
    }
}
