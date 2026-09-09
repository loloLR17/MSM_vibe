using TR2.Domain;
using TR2.Transport;

namespace TR2.Protocol;

public sealed class B5CommandWriter
{
    private readonly IRegisterWriteTransport _transport;

    public B5CommandWriter(IRegisterWriteTransport transport)
    {
        ArgumentNullException.ThrowIfNull(transport);
        _transport = transport;
    }

    public async ValueTask WriteAndSubmitAsync(
        TR2Endpoint endpoint,
        B5CommandRequest request,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(endpoint);
        ArgumentNullException.ThrowIfNull(request);

        await _transport.WriteRegistersAsync(
            endpoint.Bus.Id,
            endpoint.Address.Value,
            B5CommandRequest.StartAddress,
            request.ToPreparedRegisters(),
            cancellationToken);

        await _transport.WriteRegistersAsync(
            endpoint.Bus.Id,
            endpoint.Address.Value,
            B5CommandRequest.ControlAddress,
            [B5CommandRequest.SubmitControlValue],
            cancellationToken);
    }
}
