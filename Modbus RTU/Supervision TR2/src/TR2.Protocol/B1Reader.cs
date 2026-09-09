using TR2.Domain;
using TR2.Transport;

namespace TR2.Protocol;

public sealed class B1Reader
{
    private readonly IRegisterTransport _transport;

    public B1Reader(IRegisterTransport transport)
    {
        ArgumentNullException.ThrowIfNull(transport);
        _transport = transport;
    }

    public async ValueTask<B1SystemState> ReadAsync(
        TR2Endpoint endpoint,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(endpoint);

        var registers = await _transport.ReadRegistersAsync(
            endpoint.Bus.Id,
            endpoint.Address.Value,
            B1SystemState.StartAddress,
            B1SystemState.RegisterCount,
            cancellationToken);

        return B1SystemState.Parse(registers);
    }
}
