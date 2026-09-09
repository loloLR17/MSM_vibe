using TR2.Domain;
using TR2.Transport;

namespace TR2.Protocol;

public sealed class B0Reader : IB0SessionReader
{
    public const ushort StartAddress = 0;

    private readonly IRegisterTransport _transport;

    public B0Reader(IRegisterTransport transport)
    {
        ArgumentNullException.ThrowIfNull(transport);
        _transport = transport;
    }

    public async ValueTask<TR2Session> ReadSessionAsync(
        TR2Endpoint endpoint,
        ushort supportedProtocolVersion,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(endpoint);

        var registers = await _transport.ReadRegistersAsync(
            endpoint.Bus.Id,
            endpoint.Address.Value,
            StartAddress,
            B0Identification.RegisterCount,
            cancellationToken);

        return B0SessionFactory.Create(
            endpoint,
            registers,
            supportedProtocolVersion);
    }
}
