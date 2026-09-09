using TR2.Domain;
using TR2.Protocol;
using Xunit;

namespace TR2.Application.Tests;

public sealed class FleetDiscoveryServiceTests
{
    [Fact]
    public async Task Refresh_registers_compatible_session_from_B0()
    {
        var endpoint = Endpoint();
        var registry = RegistryWith(endpoint);
        var reader = new StubB0SessionReader(
            TR2Session.CreateCompatible(endpoint, new TR2Device(new DeviceId(1234))));
        var service = new FleetDiscoveryService(registry, reader, supportedProtocolVersion: 1);

        var refreshed = await service.RefreshAsync(endpoint);

        Assert.Equal(TR2SessionState.Compatible, refreshed.State);
        Assert.Equal(new DeviceId(1234), refreshed.Device!.DeviceId);
        Assert.Equal(refreshed, registry.GetSession(endpoint));
        Assert.Equal((ushort)1, reader.LastSupportedProtocolVersion);
    }

    [Fact]
    public async Task Refresh_registers_incompatible_session_from_B0()
    {
        var endpoint = Endpoint();
        var registry = RegistryWith(endpoint);
        var incompatible = TR2Session.CreateIncompatible(endpoint);
        var service = new FleetDiscoveryService(
            registry,
            new StubB0SessionReader(incompatible),
            supportedProtocolVersion: 7);

        var refreshed = await service.RefreshAsync(endpoint);

        Assert.Equal(TR2SessionState.Incompatible, refreshed.State);
        Assert.Null(refreshed.Device);
        Assert.Equal(refreshed, registry.GetSession(endpoint));
    }

    [Fact]
    public async Task Transport_failure_marks_known_session_disconnected_and_preserves_identity()
    {
        var endpoint = Endpoint();
        var registry = RegistryWith(endpoint);
        var known = TR2Session.CreateCompatible(endpoint, new TR2Device(new DeviceId(4321)));
        registry.SetSession(known);
        var service = new FleetDiscoveryService(
            registry,
            new ThrowingB0SessionReader(new InvalidOperationException("transport failed")),
            supportedProtocolVersion: 1);

        await Assert.ThrowsAsync<InvalidOperationException>(async () =>
            await service.RefreshAsync(endpoint));

        var disconnected = registry.GetSession(endpoint);
        Assert.Equal(TR2SessionState.Disconnected, disconnected.State);
        Assert.Equal(new DeviceId(4321), disconnected.Device!.DeviceId);
    }

    [Fact]
    public async Task Reconnect_rechecks_B0_and_can_identify_a_different_device_at_same_endpoint()
    {
        var endpoint = Endpoint();
        var registry = RegistryWith(endpoint);
        registry.SetSession(
            TR2Session.CreateCompatible(endpoint, new TR2Device(new DeviceId(1001))).MarkDisconnected());

        var replacement = TR2Session.CreateCompatible(
            endpoint,
            new TR2Device(new DeviceId(2002)));
        var service = new FleetDiscoveryService(
            registry,
            new StubB0SessionReader(replacement),
            supportedProtocolVersion: 1);

        var refreshed = await service.RefreshAsync(endpoint);

        Assert.Equal(new DeviceId(2002), refreshed.Device!.DeviceId);
        Assert.Equal(refreshed, registry.GetSession(endpoint));
    }

    [Fact]
    public async Task Cancellation_does_not_reclassify_session_as_transport_failure()
    {
        var endpoint = Endpoint();
        var registry = RegistryWith(endpoint);
        var known = TR2Session.CreateCompatible(endpoint, new TR2Device(new DeviceId(999)));
        registry.SetSession(known);
        using var cts = new CancellationTokenSource();
        cts.Cancel();
        var service = new FleetDiscoveryService(
            registry,
            new CancellingB0SessionReader(),
            supportedProtocolVersion: 1);

        await Assert.ThrowsAnyAsync<OperationCanceledException>(async () =>
            await service.RefreshAsync(endpoint, cts.Token));

        Assert.Equal(known, registry.GetSession(endpoint));
    }

    private static FleetRegistry RegistryWith(TR2Endpoint endpoint)
    {
        var registry = new FleetRegistry();
        registry.RegisterEndpoint(endpoint);
        return registry;
    }

    private static TR2Endpoint Endpoint() =>
        new(new SerialBus("RS485-A"), new ModbusAddress(10));

    private sealed class StubB0SessionReader : IB0SessionReader
    {
        private readonly TR2Session _session;

        public StubB0SessionReader(TR2Session session)
        {
            _session = session;
        }

        public ushort? LastSupportedProtocolVersion { get; private set; }

        public ValueTask<TR2Session> ReadSessionAsync(
            TR2Endpoint endpoint,
            ushort supportedProtocolVersion,
            CancellationToken cancellationToken = default)
        {
            LastSupportedProtocolVersion = supportedProtocolVersion;
            return ValueTask.FromResult(_session);
        }
    }

    private sealed class ThrowingB0SessionReader : IB0SessionReader
    {
        private readonly Exception _exception;

        public ThrowingB0SessionReader(Exception exception)
        {
            _exception = exception;
        }

        public ValueTask<TR2Session> ReadSessionAsync(
            TR2Endpoint endpoint,
            ushort supportedProtocolVersion,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromException<TR2Session>(_exception);
    }

    private sealed class CancellingB0SessionReader : IB0SessionReader
    {
        public ValueTask<TR2Session> ReadSessionAsync(
            TR2Endpoint endpoint,
            ushort supportedProtocolVersion,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromCanceled<TR2Session>(cancellationToken);
    }
}
