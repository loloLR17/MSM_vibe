using TR2.Domain;
using TR2.Transport;

namespace TR2.Protocol;

public sealed class B6CampaignSelector
{
    public const ushort SelectedCampaignIndexAddress = 6003;

    private readonly IRegisterWriteTransport _transport;

    public B6CampaignSelector(IRegisterWriteTransport transport)
    {
        ArgumentNullException.ThrowIfNull(transport);
        _transport = transport;
    }

    public ValueTask SelectAsync(
        TR2Endpoint endpoint,
        ushort campaignIndex,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(endpoint);

        return _transport.WriteRegistersAsync(
            endpoint.Bus.Id,
            endpoint.Address.Value,
            SelectedCampaignIndexAddress,
            new ushort[] { campaignIndex },
            cancellationToken);
    }
}
