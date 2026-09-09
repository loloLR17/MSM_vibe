using TR2.Protocol;
using TR2.Transport;

namespace TR2.Application;

public sealed class PollingExecutor
{
    private readonly ushort _supportedProtocolVersion;
    private readonly B0Reader _b0;
    private readonly B1Reader _b1;
    private readonly B2Reader _b2;
    private readonly B3Reader _b3;
    private readonly B4Reader _b4;
    private readonly B5Reader _b5;
    private readonly B6Reader _b6;
    private readonly B7Reader _b7;

    public PollingExecutor(IRegisterTransport transport, ushort supportedProtocolVersion)
    {
        ArgumentNullException.ThrowIfNull(transport);
        _supportedProtocolVersion = supportedProtocolVersion;
        _b0 = new B0Reader(transport);
        _b1 = new B1Reader(transport);
        _b2 = new B2Reader(transport);
        _b3 = new B3Reader(transport);
        _b4 = new B4Reader(transport);
        _b5 = new B5Reader(transport);
        _b6 = new B6Reader(transport);
        _b7 = new B7Reader(transport);
    }

    public async ValueTask<PollingReadSet> ExecuteAsync(
        ScheduledBusWork work,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(work);

        if (work.Kind != BusWorkKind.Polling || work.PollingGroup is null)
        {
            throw new InvalidOperationException("The work item is not a polling work item.");
        }

        var endpoint = work.Endpoint;
        return work.PollingGroup.Value switch
        {
            PollingGroup.Fast => new PollingReadSet(
                PollingGroup.Fast,
                null,
                await _b1.ReadAsync(endpoint, cancellationToken),
                null,
                await _b3.ReadAsync(endpoint, cancellationToken),
                null,
                await _b5.ReadAsync(endpoint, cancellationToken),
                null,
                null),

            PollingGroup.Medium => new PollingReadSet(
                PollingGroup.Medium,
                null,
                null,
                await _b2.ReadAsync(endpoint, cancellationToken),
                null,
                null,
                null,
                null,
                await _b7.ReadAsync(endpoint, cancellationToken)),

            PollingGroup.Slow => new PollingReadSet(
                PollingGroup.Slow,
                null,
                null,
                null,
                null,
                await _b4.ReadAsync(endpoint, cancellationToken),
                null,
                await _b6.ReadAsync(endpoint, cancellationToken),
                null),

            PollingGroup.Static => new PollingReadSet(
                PollingGroup.Static,
                await _b0.ReadSessionAsync(endpoint, _supportedProtocolVersion, cancellationToken),
                null,
                null,
                null,
                null,
                null,
                null,
                null),

            _ => throw new ArgumentOutOfRangeException(nameof(work.PollingGroup))
        };
    }
}
