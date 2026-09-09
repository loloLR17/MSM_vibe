namespace TR2.Transport;

public sealed class ModbusRegisterTransport : IRegisterTransport, IRegisterWriteTransport
{
    private readonly string _busId;
    private readonly IModbusRegisterClient _client;

    public ModbusRegisterTransport(string busId, IModbusRegisterClient client)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(busId);
        ArgumentNullException.ThrowIfNull(client);

        _busId = busId;
        _client = client;
    }

    public async ValueTask<ushort[]> ReadRegistersAsync(
        string busId,
        byte unitAddress,
        ushort startAddress,
        ushort registerCount,
        CancellationToken cancellationToken = default)
    {
        ValidateBus(busId);
        cancellationToken.ThrowIfCancellationRequested();

        return await _client
            .ReadHoldingRegistersAsync(unitAddress, startAddress, registerCount)
            .ConfigureAwait(false);
    }

    public async ValueTask WriteRegistersAsync(
        string busId,
        byte unitAddress,
        ushort startAddress,
        IReadOnlyList<ushort> values,
        CancellationToken cancellationToken = default)
    {
        ValidateBus(busId);
        ArgumentNullException.ThrowIfNull(values);
        cancellationToken.ThrowIfCancellationRequested();

        if (values.Count == 0)
        {
            throw new ArgumentException("At least one register value must be provided.", nameof(values));
        }

        await _client
            .WriteMultipleRegistersAsync(unitAddress, startAddress, values.ToArray())
            .ConfigureAwait(false);
    }

    private void ValidateBus(string busId)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(busId);

        if (!string.Equals(busId, _busId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"Transport for bus '{_busId}' cannot execute work for bus '{busId}'.");
        }
    }
}
