using TR2.Domain;
using TR2.Transport;

namespace TR2.Protocol;

public sealed class B5CommandWriter : IB5CommandWriter
{
    private readonly IRegisterWriteTransport _transport;

    public B5CommandWriter(IRegisterWriteTransport transport)
    {
        ArgumentNullException.ThrowIfNull(transport);
        _transport = transport;
    }

    public ValueTask PrepareAsync(
        TR2Endpoint endpoint,
        B5CommandRequest request,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(endpoint);
        ArgumentNullException.ThrowIfNull(request);

        return _transport.WriteRegistersAsync(
            endpoint.Bus.Id,
            endpoint.Address.Value,
            B5CommandRequest.StartAddress,
            request.ToPreparedRegisters(),
            cancellationToken);
    }

    public ValueTask SubmitAsync(
        TR2Endpoint endpoint,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(endpoint);

        return _transport.WriteRegistersAsync(
            endpoint.Bus.Id,
            endpoint.Address.Value,
            B5CommandRequest.ControlAddress,
            [B5CommandRequest.SubmitControlValue],
            cancellationToken);
    }

    public async ValueTask WriteAndSubmitAsync(
        TR2Endpoint endpoint,
        B5CommandRequest request,
        CancellationToken cancellationToken = default)
    {
        await PrepareAsync(endpoint, request, cancellationToken);
        await SubmitAsync(endpoint, cancellationToken);
    }
}
