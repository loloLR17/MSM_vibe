using TR2.Domain;
using TR2.Transport;

namespace TR2.Protocol;

public sealed class B6Reader
{
    private readonly IRegisterTransport _transport;

    public B6Reader(IRegisterTransport transport)
    {
        ArgumentNullException.ThrowIfNull(transport);
        _transport = transport;
    }

    public async ValueTask<B6CampaignInventoryState> ReadAsync(
        TR2Endpoint endpoint,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(endpoint);

        var registers = await _transport.ReadRegistersAsync(
            endpoint.Bus.Id,
            endpoint.Address.Value,
            B6CampaignInventoryState.StartAddress,
            B6CampaignInventoryState.RegisterCount,
            cancellationToken);

        return B6CampaignInventoryState.Parse(registers);
    }
}
