using TR2.Domain;
using Xunit;

namespace TR2.Application.Tests;

public sealed class FleetRegistryTests
{
    [Fact]
    public void Registering_endpoint_creates_one_unidentified_current_session()
    {
        var registry = new FleetRegistry();
        var endpoint = Endpoint("RS485-A", 12);

        var session = registry.RegisterEndpoint(endpoint);

        Assert.Equal(TR2SessionState.Unidentified, session.State);
        Assert.Equal(session, registry.GetSession(endpoint));
        Assert.Single(registry.Sessions);
    }

    [Fact]
    public void Duplicate_endpoint_registration_is_rejected()
    {
        var registry = new FleetRegistry();
        var endpoint = Endpoint("RS485-A", 12);
        registry.RegisterEndpoint(endpoint);

        Assert.Throws<InvalidOperationException>(() => registry.RegisterEndpoint(endpoint));
    }

    [Fact]
    public void Current_session_can_be_replaced_after_identification()
    {
        var registry = new FleetRegistry();
        var endpoint = Endpoint("RS485-A", 12);
        var previous = registry.RegisterEndpoint(endpoint);
        var identified = TR2Session.CreateCompatible(
            endpoint,
            new TR2Device(new DeviceId(1001)));

        registry.SetSession(identified);

        Assert.Equal(identified, registry.GetSession(endpoint));
        Assert.Equal(TR2SessionState.Unidentified, previous.State);
        Assert.Null(previous.Device);
    }

    [Fact]
    public void Same_device_cannot_be_compatible_on_two_endpoints_at_once()
    {
        var registry = new FleetRegistry();
        var firstEndpoint = Endpoint("RS485-A", 12);
        var secondEndpoint = Endpoint("RS485-A", 13);
        registry.RegisterEndpoint(firstEndpoint);
        registry.RegisterEndpoint(secondEndpoint);
        registry.SetSession(TR2Session.CreateCompatible(
            firstEndpoint,
            new TR2Device(new DeviceId(1001))));

        Assert.Throws<InvalidOperationException>(() =>
            registry.SetSession(TR2Session.CreateCompatible(
                secondEndpoint,
                new TR2Device(new DeviceId(1001)))));
    }

    [Fact]
    public void Device_can_move_after_previous_session_is_disconnected()
    {
        var registry = new FleetRegistry();
        var firstEndpoint = Endpoint("RS485-A", 12);
        var secondEndpoint = Endpoint("RS485-B", 7);
        registry.RegisterEndpoint(firstEndpoint);
        registry.RegisterEndpoint(secondEndpoint);

        var firstSession = TR2Session.CreateCompatible(
            firstEndpoint,
            new TR2Device(new DeviceId(1001)));
        registry.SetSession(firstSession);
        registry.SetSession(firstSession.MarkDisconnected());

        var moved = TR2Session.CreateCompatible(
            secondEndpoint,
            new TR2Device(new DeviceId(1001)));
        registry.SetSession(moved);

        Assert.Equal(TR2SessionState.Disconnected, registry.GetSession(firstEndpoint).State);
        Assert.Equal(moved, registry.GetSession(secondEndpoint));
    }

    [Fact]
    public void Same_endpoint_can_identify_a_replacement_device_without_mutating_old_session()
    {
        var registry = new FleetRegistry();
        var endpoint = Endpoint("RS485-A", 12);
        registry.RegisterEndpoint(endpoint);

        var first = TR2Session.CreateCompatible(
            endpoint,
            new TR2Device(new DeviceId(1001)));
        registry.SetSession(first);

        var replacement = TR2Session.CreateCompatible(
            endpoint,
            new TR2Device(new DeviceId(2002)));
        registry.SetSession(replacement);

        Assert.Equal(new DeviceId(1001), first.Device!.DeviceId);
        Assert.Equal(new DeviceId(2002), registry.GetSession(endpoint).Device!.DeviceId);
    }

    [Fact]
    public void Session_for_unknown_endpoint_is_rejected()
    {
        var registry = new FleetRegistry();
        var endpoint = Endpoint("RS485-A", 12);
        var session = TR2Session.CreateCompatible(
            endpoint,
            new TR2Device(new DeviceId(1001)));

        Assert.Throws<InvalidOperationException>(() => registry.SetSession(session));
        Assert.Throws<KeyNotFoundException>(() => registry.GetSession(endpoint));
    }

    private static TR2Endpoint Endpoint(string bus, byte address) =>
        new(new SerialBus(bus), new ModbusAddress(address));
}
