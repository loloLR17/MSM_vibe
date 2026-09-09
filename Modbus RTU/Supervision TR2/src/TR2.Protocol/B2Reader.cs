using TR2.Domain;
using TR2.Transport;

namespace TR2.Protocol;

public sealed class B2Reader
{
    private readonly IRegisterTransport _transport;

    public B2Reader(IRegisterTransport transport)
    {
        ArgumentNullException.ThrowIfNull(transport);
        _transport = transport;
    }

    public async ValueTask<B2TimeState> ReadAsync(
        TR2Endpoint endpoint,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(endpoint);

        var registers = await _transport.ReadRegistersAsync(
            endpoint.Bus.Id,
            endpoint.Address.Value,
            B2TimeState.StartAddress,
            B2TimeState.RegisterCount,
            cancellationToken);

        return B2TimeState.Parse(registers);
    }
}
