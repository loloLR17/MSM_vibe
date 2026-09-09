namespace TR2.Transport;

public sealed class ModbusBusConnectionManager : IAsyncDisposable
{
    private readonly IModbusBusConnectionFactory _factory;
    private readonly Dictionary<string, IModbusBusConnection> _connections = new(StringComparer.Ordinal);
    private bool _disposed;

    public ModbusBusConnectionManager(IModbusBusConnectionFactory factory)
    {
        ArgumentNullException.ThrowIfNull(factory);
        _factory = factory;
    }

    public IReadOnlyCollection<string> ConnectedBusIds => _connections.Keys;

    public async ValueTask<IModbusBusConnection> OpenAsync(
        string busId,
        string portName,
        CancellationToken cancellationToken = default)
    {
        ThrowIfDisposed();
        ArgumentException.ThrowIfNullOrWhiteSpace(busId);
        ArgumentException.ThrowIfNullOrWhiteSpace(portName);
        cancellationToken.ThrowIfCancellationRequested();

        if (_connections.ContainsKey(busId))
        {
            throw new InvalidOperationException($"Bus '{busId}' already has an open Modbus connection.");
        }

        var connection = await _factory
            .OpenAsync(busId, portName, cancellationToken)
            .ConfigureAwait(false);

        if (!string.Equals(connection.BusId, busId, StringComparison.Ordinal))
        {
            await connection.DisposeAsync().ConfigureAwait(false);
            throw new InvalidOperationException(
                $"Connection factory returned bus '{connection.BusId}' for requested bus '{busId}'.");
        }

        _connections.Add(busId, connection);
        return connection;
    }

    public bool TryGet(string busId, out IModbusBusConnection? connection)
    {
        ThrowIfDisposed();
        ArgumentException.ThrowIfNullOrWhiteSpace(busId);
        return _connections.TryGetValue(busId, out connection);
    }

    public async ValueTask<bool> CloseAsync(string busId)
    {
        ThrowIfDisposed();
        ArgumentException.ThrowIfNullOrWhiteSpace(busId);

        if (!_connections.Remove(busId, out var connection))
        {
            return false;
        }

        await connection.DisposeAsync().ConfigureAwait(false);
        return true;
    }

    public async ValueTask DisposeAsync()
    {
        if (_disposed)
        {
            return;
        }

        _disposed = true;
        var connections = _connections.Values.ToArray();
        _connections.Clear();

        foreach (var connection in connections)
        {
            await connection.DisposeAsync().ConfigureAwait(false);
        }
    }

    private void ThrowIfDisposed()
    {
        ObjectDisposedException.ThrowIf(_disposed, this);
    }
}
