using TR2.Domain;
using TR2.Transport;

namespace TR2.Protocol;

public sealed class B5Reader
{
    private readonly IRegisterTransport _transport;

    public B5Reader(IRegisterTransport transport)
    {
        ArgumentNullException.ThrowIfNull(transport);
        _transport = transport;
    }

    public async ValueTask<B5CommandState> ReadAsync(
        TR2Endpoint endpoint,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(endpoint);

        var registers = await _transport.ReadRegistersAsync(
            endpoint.Bus.Id,
            endpoint.Address.Value,
            B5CommandState.StartAddress,
            B5CommandState.RegisterCount,
            cancellationToken);

        return B5CommandState.Parse(registers);
    }
}
