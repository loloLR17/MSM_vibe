using TR2.Domain;

namespace TR2.Application;

public sealed class FleetRegistry
{
    private readonly Dictionary<TR2Endpoint, TR2Session> _sessions = [];

    public IReadOnlyCollection<TR2Session> Sessions => _sessions.Values;

    public TR2Session RegisterEndpoint(TR2Endpoint endpoint)
    {
        ArgumentNullException.ThrowIfNull(endpoint);

        if (_sessions.ContainsKey(endpoint))
        {
            throw new InvalidOperationException("Endpoint is already registered.");
        }

        var session = TR2Session.CreateUnidentified(endpoint);
        _sessions.Add(endpoint, session);
        return session;
    }

    public TR2Session GetSession(TR2Endpoint endpoint)
    {
        ArgumentNullException.ThrowIfNull(endpoint);

        return _sessions.TryGetValue(endpoint, out var session)
            ? session
            : throw new KeyNotFoundException("Endpoint is not registered.");
    }

    public void SetSession(TR2Session session)
    {
        ArgumentNullException.ThrowIfNull(session);

        if (!_sessions.ContainsKey(session.Endpoint))
        {
            throw new InvalidOperationException("Endpoint must be registered before assigning a session.");
        }

        EnsureNoDuplicateCompatibleDevice(session);
        _sessions[session.Endpoint] = session;
    }

    private void EnsureNoDuplicateCompatibleDevice(TR2Session candidate)
    {
        if (candidate.State != TR2SessionState.Compatible || candidate.Device is null)
        {
            return;
        }

        var duplicate = _sessions.Values.Any(existing =>
            existing.Endpoint != candidate.Endpoint &&
            existing.State == TR2SessionState.Compatible &&
            existing.Device?.DeviceId == candidate.Device.DeviceId);

        if (duplicate)
        {
            throw new InvalidOperationException(
                "A device_id cannot be active as Compatible on multiple endpoints.");
        }
    }
}
