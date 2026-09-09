using TR2.Domain;
using Xunit;

namespace TR2.Protocol.Tests;

public sealed class B0IdentificationTests
{
    [Fact]
    public void Parser_reconstructs_device_id_from_msw_then_lsw()
    {
        var registers = Registers();
        registers[B0Identification.DeviceIdMswOffset] = 0x1234;
        registers[B0Identification.DeviceIdLswOffset] = 0xABCD;
        registers[B0Identification.ProtocolVersionOffset] = 7;

        var identification = B0Identification.Parse(registers);

        Assert.Equal(new DeviceId(0x1234ABCD), identification.DeviceId);
        Assert.Equal((ushort)7, identification.ProtocolVersion);
    }

    [Fact]
    public void Parser_preserves_full_uint32_device_id_domain()
    {
        var zero = Registers();
        var maximum = Registers();
        maximum[B0Identification.DeviceIdMswOffset] = ushort.MaxValue;
        maximum[B0Identification.DeviceIdLswOffset] = ushort.MaxValue;

        Assert.Equal(new DeviceId(0), B0Identification.Parse(zero).DeviceId);
        Assert.Equal(new DeviceId(uint.MaxValue), B0Identification.Parse(maximum).DeviceId);
    }

    [Fact]
    public void Parser_requires_complete_B0_register_image()
    {
        var incomplete = new ushort[B0Identification.RegisterCount - 1];

        Assert.Throws<ArgumentException>(() => B0Identification.Parse(incomplete));
    }

    [Fact]
    public void Protocol_compatibility_is_exact_and_caller_supplied()
    {
        var registers = Registers();
        registers[B0Identification.ProtocolVersionOffset] = 12;
        var identification = B0Identification.Parse(registers);

        Assert.True(identification.SupportsProtocolVersion(12));
        Assert.False(identification.SupportsProtocolVersion(11));
    }

    [Fact]
    public void Supported_B0_creates_compatible_identified_session()
    {
        var endpoint = Endpoint();
        var registers = Registers();
        registers[B0Identification.DeviceIdMswOffset] = 0x0001;
        registers[B0Identification.DeviceIdLswOffset] = 0x002A;
        registers[B0Identification.ProtocolVersionOffset] = 4;

        var session = B0SessionFactory.Create(endpoint, registers, supportedProtocolVersion: 4);

        Assert.Equal(TR2SessionState.Compatible, session.State);
        Assert.Equal(new DeviceId(0x0001002A), session.Device!.DeviceId);
        Assert.Equal(endpoint, session.Endpoint);
    }

    [Fact]
    public void Unsupported_protocol_creates_incompatible_session_without_invented_identity()
    {
        var endpoint = Endpoint();
        var registers = Registers();
        registers[B0Identification.DeviceIdMswOffset] = 0x0001;
        registers[B0Identification.DeviceIdLswOffset] = 0x002A;
        registers[B0Identification.ProtocolVersionOffset] = 5;

        var session = B0SessionFactory.Create(endpoint, registers, supportedProtocolVersion: 4);

        Assert.Equal(TR2SessionState.Incompatible, session.State);
        Assert.Null(session.Device);
        Assert.Equal(endpoint, session.Endpoint);
    }

    private static ushort[] Registers() => new ushort[B0Identification.RegisterCount];

    private static TR2Endpoint Endpoint() =>
        new(new SerialBus("RS485-A"), new ModbusAddress(10));
}
