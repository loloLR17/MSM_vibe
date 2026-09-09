namespace TR2.Transport;

public interface IModbusBusConnectionFactory
{
    ValueTask<IModbusBusConnection> OpenAsync(
        string busId,
        string portName,
        CancellationToken cancellationToken = default);
}
