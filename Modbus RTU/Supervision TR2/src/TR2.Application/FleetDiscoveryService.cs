using TR2.Domain;
using TR2.Protocol;

namespace TR2.Application;

public sealed class FleetDiscoveryService
{
    private readonly FleetRegistry _fleetRegistry;
    private readonly IB0SessionReader _b0SessionReader;
    private readonly ushort _supportedProtocolVersion;
    private readonly FleetRefreshPlanner? _refreshPlanner;
    private readonly RecoveredCommandReconciliationPlanner? _reconciliationPlanner;

    public FleetDiscoveryService(
        FleetRegistry fleetRegistry,
        IB0SessionReader b0SessionReader,
        ushort supportedProtocolVersion,
        FleetRefreshPlanner? refreshPlanner = null,
        RecoveredCommandReconciliationPlanner? reconciliationPlanner = null)
    {
        ArgumentNullException.ThrowIfNull(fleetRegistry);
        ArgumentNullException.ThrowIfNull(b0SessionReader);

        _fleetRegistry = fleetRegistry;
        _b0SessionReader = b0SessionReader;
        _supportedProtocolVersion = supportedProtocolVersion;
        _refreshPlanner = refreshPlanner;
        _reconciliationPlanner = reconciliationPlanner;
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

    public async ValueTask<(TR2Session Session, IReadOnlyList<ScheduledBlockRefresh> Refreshes)> ReconnectAsync(
        TR2Endpoint endpoint,
        DateTimeOffset dueAt,
        CancellationToken cancellationToken = default)
    {
        if (_refreshPlanner is null)
        {
            throw new InvalidOperationException("A refresh planner is required for reconnect orchestration.");
        }

        var session = await RefreshAsync(endpoint, cancellationToken);

        if (session.State == TR2SessionState.Compatible)
        {
            _reconciliationPlanner?.QueueForSession(session, dueAt);
        }

        var refreshes = session.State == TR2SessionState.Compatible
            ? _refreshPlanner.QueuePostReconnectRefresh(session, dueAt)
            : Array.Empty<ScheduledBlockRefresh>();

        return (session, refreshes);
    }
}
