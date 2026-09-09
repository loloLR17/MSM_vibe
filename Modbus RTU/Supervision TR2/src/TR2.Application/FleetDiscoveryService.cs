using TR2.Domain;
using TR2.Protocol;

namespace TR2.Application;

public sealed class FleetDiscoveryService
{
    private readonly FleetRegistry _fleetRegistry;
    private readonly IB0SessionReader _b0SessionReader;
    private readonly ushort _supportedProtocolVersion;

    public FleetDiscoveryService(
        FleetRegistry fleetRegistry,
        IB0SessionReader b0SessionReader,
        ushort supportedProtocolVersion)
    {
        ArgumentNullException.ThrowIfNull(fleetRegistry);
        ArgumentNullException.ThrowIfNull(b0SessionReader);

        _fleetRegistry = fleetRegistry;
        _b0SessionReader = b0SessionReader;
        _supportedProtocolVersion = supportedProtocolVersion;
    }

    public async ValueTask<TR2Session> RefreshAsync(
        TR2Endpoint endpoint,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(endpoint);

        var current = _fleetRegistry.GetSession(endpoint);

        try
        {
            var refreshed = await _b0SessionReader.ReadSessionAsync(
                endpoint,
                _supportedProtocolVersion,
                cancellationToken);

            _fleetRegistry.SetSession(refreshed);
            return refreshed;
        }
        catch (OperationCanceledException) when (cancellationToken.IsCancellationRequested)
        {
            throw;
        }
        catch
        {
            _fleetRegistry.SetSession(current.MarkDisconnected());
            throw;
        }
    }
}
