using TR2.Domain;
using TR2.Transport;

namespace TR2.Protocol;

public sealed class B3Reader
{
    private readonly IRegisterTransport _transport;

    public B3Reader(IRegisterTransport transport)
    {
        ArgumentNullException.ThrowIfNull(transport);
        _transport = transport;
    }

    public async ValueTask<B3VibrationSupervision> ReadAsync(
        TR2Endpoint endpoint,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(endpoint);

        var registers = await _transport.ReadRegistersAsync(
            endpoint.Bus.Id,
            endpoint.Address.Value,
            B3VibrationSupervision.StartAddress,
            B3VibrationSupervision.RegisterCount,
            cancellationToken);

        return B3VibrationSupervision.Parse(registers);
    }
}
