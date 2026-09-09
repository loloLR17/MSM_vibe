namespace TR2.Transport;

public interface IRegisterTransport
{
    ValueTask<ushort[]> ReadRegistersAsync(
        string busId,
        byte unitAddress,
        ushort startAddress,
        ushort registerCount,
        CancellationToken cancellationToken = default);
}
