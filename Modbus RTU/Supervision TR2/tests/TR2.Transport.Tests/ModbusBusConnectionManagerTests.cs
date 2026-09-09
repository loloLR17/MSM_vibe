using TR2.Transport;
using Xunit;

namespace TR2.Transport.Tests;

public sealed class ModbusBusConnectionManagerTests
{
    [Fact]
    public async Task OpenRegistersConnectionForRequestedBus()
    {
        var factory = new FakeFactory();
        await using var manager = new ModbusBusConnectionManager(factory);

        var connection = await manager.OpenAsync("bus-a", "COM7");

        Assert.Equal("bus-a", connection.BusId);
        Assert.True(manager.TryGet("bus-a", out var registered));
        Assert.Same(connection, registered);
        Assert.Equal(new[] { "bus-a" }, manager.ConnectedBusIds);
        Assert.Equal(("bus-a", "COM7"), factory.OpenRequests.Single());
    }

    [Fact]
    public async Task OpenRejectsSecondConnectionForSameBus()
    {
        var factory = new FakeFactory();
        await using var manager = new ModbusBusConnectionManager(factory);
        await manager.OpenAsync("bus-a", "COM7");

        await Assert.ThrowsAsync<InvalidOperationException>(async () =>
            await manager.OpenAsync("bus-a", "COM8"));

        Assert.Single(factory.OpenRequests);
    }

    [Fact]
    public async Task OpenDoesNotRegisterConnectionWhenFactoryFails()
    {
        var factory = new FakeFactory
        {
            Failure = new IOException("port unavailable")
        };
        await using var manager = new ModbusBusConnectionManager(factory);

        await Assert.ThrowsAsync<IOException>(async () =>
            await manager.OpenAsync("bus-a", "COM7"));

        Assert.False(manager.TryGet("bus-a", out _));
        Assert.Empty(manager.ConnectedBusIds);
    }

    [Fact]
    public async Task OpenDisposesFactoryConnectionWhenBusIdentityDoesNotMatch()
    {
        var wrongConnection = new FakeConnection("bus-b");
        var factory = new FakeFactory
        {
            ConnectionFactory = (_, _) => wrongConnection
        };
        await using var manager = new ModbusBusConnectionManager(factory);

        await Assert.ThrowsAsync<InvalidOperationException>(async () =>
            await manager.OpenAsync("bus-a", "COM7"));

        Assert.True(wrongConnection.Disposed);
        Assert.Empty(manager.ConnectedBusIds);
    }

    [Fact]
    public async Task CloseRemovesAndDisposesConnection()
    {
        var factory = new FakeFactory();
        await using var manager = new ModbusBusConnectionManager(factory);
        var connection = (FakeConnection)await manager.OpenAsync("bus-a", "COM7");

        var closed = await manager.CloseAsync("bus-a");

        Assert.True(closed);
        Assert.True(connection.Disposed);
        Assert.False(manager.TryGet("bus-a", out _));
        Assert.False(await manager.CloseAsync("bus-a"));
    }

    [Fact]
    public async Task DisposeClosesEveryRegisteredConnection()
    {
        var factory = new FakeFactory();
        var manager = new ModbusBusConnectionManager(factory);
        var first = (FakeConnection)await manager.OpenAsync("bus-a", "COM7");
        var second = (FakeConnection)await manager.OpenAsync("bus-b", "COM8");

        await manager.DisposeAsync();

        Assert.True(first.Disposed);
        Assert.True(second.Disposed);
        await Assert.ThrowsAsync<ObjectDisposedException>(async () =>
            await manager.OpenAsync("bus-c", "COM9"));
    }

    [Fact]
    public async Task CancelledOpenDoesNotCallFactory()
    {
        var factory = new FakeFactory();
        await using var manager = new ModbusBusConnectionManager(factory);
        using var cancellation = new CancellationTokenSource();
        cancellation.Cancel();

        await Assert.ThrowsAnyAsync<OperationCanceledException>(async () =>
            await manager.OpenAsync("bus-a", "COM7", cancellation.Token));

        Assert.Empty(factory.OpenRequests);
    }

    private sealed class FakeFactory : IModbusBusConnectionFactory
    {
        public List<(string BusId, string PortName)> OpenRequests { get; } = [];
        public Exception? Failure { get; init; }
        public Func<string, string, IModbusBusConnection>? ConnectionFactory { get; init; }

        public ValueTask<IModbusBusConnection> OpenAsync(
            string busId,
            string portName,
            CancellationToken cancellationToken = default)
        {
            OpenRequests.Add((busId, portName));

            if (Failure is not null)
            {
                return ValueTask.FromException<IModbusBusConnection>(Failure);
            }

            var connection = ConnectionFactory?.Invoke(busId, portName) ?? new FakeConnection(busId);
            return ValueTask.FromResult(connection);
        }
    }

    private sealed class FakeConnection : IModbusBusConnection
    {
        public FakeConnection(string busId)
        {
            BusId = busId;
            var transport = new NullRegisterTransport();
            RegisterTransport = transport;
            RegisterWriteTransport = transport;
        }

        public string BusId { get; }
        public IRegisterTransport RegisterTransport { get; }
        public IRegisterWriteTransport RegisterWriteTransport { get; }
        public bool Disposed { get; private set; }

        public ValueTask DisposeAsync()
        {
            Disposed = true;
            return ValueTask.CompletedTask;
        }
    }

    private sealed class NullRegisterTransport : IRegisterTransport, IRegisterWriteTransport
    {
        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromResult(Array.Empty<ushort>());

        public ValueTask WriteRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            IReadOnlyList<ushort> values,
            CancellationToken cancellationToken = default) =>
            ValueTask.CompletedTask;
    }
}
