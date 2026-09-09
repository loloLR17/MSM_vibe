namespace TR2.Transport;

public interface IModbusBusConnectionFactory
{
    ValueTask<IModbusBusConnection> OpenAsync(
        string busId,
        ModbusSerialConnectionSettings settings,
        CancellationToken cancellationToken = default);
}
