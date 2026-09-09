using Xunit;

namespace TR2.Domain.Tests;

public sealed class FleetIdentityTests
{
    [Theory]
    [InlineData(null)]
    [InlineData("")]
    [InlineData("   ")]
    public void SerialBus_rejects_missing_identity(string? id)
    {
        Assert.ThrowsAny<ArgumentException>(() => new SerialBus(id!));
    }

    [Fact]
    public void DeviceId_preserves_full_uint32_domain()
    {
        Assert.Equal(0U, new DeviceId(0U).Value);
        Assert.Equal(uint.MaxValue, new DeviceId(uint.MaxValue).Value);
    }

    [Fact]
    public void Endpoint_identity_is_bus_plus_address()
    {
        var endpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(12));
        var sameEndpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(12));
        var otherAddress = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(13));
        var otherBus = new TR2Endpoint(new SerialBus("RS485-B"), new ModbusAddress(12));

        Assert.Equal(endpoint, sameEndpoint);
        Assert.NotEqual(endpoint, otherAddress);
        Assert.NotEqual(endpoint, otherBus);
    }

    [Fact]
    public void Same_endpoint_can_identify_a_different_device_in_another_session()
    {
        var endpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(12));
        var first = TR2Session.CreateCompatible(endpoint, new TR2Device(new DeviceId(1001)));
        var replacement = TR2Session.CreateCompatible(endpoint, new TR2Device(new DeviceId(2002)));

        Assert.Equal(first.Endpoint, replacement.Endpoint);
        Assert.NotEqual(first.Device, replacement.Device);
    }

    [Fact]
    public void Compatible_session_has_identified_device()
    {
        var endpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(12));
        var device = new TR2Device(new DeviceId(1001));

        var session = TR2Session.CreateCompatible(endpoint, device);

        Assert.Equal(TR2SessionState.Compatible, session.State);
        Assert.Equal(device, session.Device);
    }

    [Fact]
    public void Communication_loss_keeps_known_device_identity()
    {
        var endpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(12));
        var device = new TR2Device(new DeviceId(1001));
        var session = TR2Session.CreateCompatible(endpoint, device);

        var disconnected = session.MarkDisconnected();

        Assert.Equal(TR2SessionState.Disconnected, disconnected.State);
        Assert.Equal(device, disconnected.Device);
        Assert.Equal(endpoint, disconnected.Endpoint);
    }

    [Fact]
    public void Unidentified_and_incompatible_sessions_do_not_invent_device_identity()
    {
        var endpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(12));

        var unidentified = TR2Session.CreateUnidentified(endpoint);
        var incompatible = TR2Session.CreateIncompatible(endpoint);

        Assert.Null(unidentified.Device);
        Assert.Null(incompatible.Device);
        Assert.Equal(TR2SessionState.Unidentified, unidentified.State);
        Assert.Equal(TR2SessionState.Incompatible, incompatible.State);
    }
}
