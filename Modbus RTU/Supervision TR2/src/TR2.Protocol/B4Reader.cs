using TR2.Domain;
using TR2.Transport;

namespace TR2.Protocol;

public sealed class B4Reader
{
    private readonly IRegisterTransport _transport;

    public B4Reader(IRegisterTransport transport)
    {
        ArgumentNullException.ThrowIfNull(transport);
        _transport = transport;
    }

    public async ValueTask<B4ConfigurationState> ReadAsync(
        TR2Endpoint endpoint,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(endpoint);

        var registers = await _transport.ReadRegistersAsync(
            endpoint.Bus.Id,
            endpoint.Address.Value,
            B4ConfigurationState.StartAddress,
            B4ConfigurationState.RegisterCount,
            cancellationToken);

        return B4ConfigurationState.Parse(registers);
    }
}
