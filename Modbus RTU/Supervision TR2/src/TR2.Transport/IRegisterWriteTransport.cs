namespace TR2.Transport;

public interface IRegisterWriteTransport
{
    ValueTask WriteRegistersAsync(
        string busId,
        byte unitAddress,
        ushort startAddress,
        IReadOnlyList<ushort> values,
        CancellationToken cancellationToken = default);
}
