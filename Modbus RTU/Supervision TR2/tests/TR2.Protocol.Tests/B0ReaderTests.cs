using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Protocol.Tests;

public sealed class B0ReaderTests
{
    [Fact]
    public async Task Reader_requests_exact_B0_range_from_generic_transport()
    {
        var transport = new FakeRegisterTransport();
        var reader = new B0Reader(transport);
        var endpoint = Endpoint("RS485-A", 10);
        transport.Registers[B0Identification.DeviceIdLswOffset] = 42;
        transport.Registers[B0Identification.ProtocolVersionOffset] = 4;

        var session = await reader.ReadSessionAsync(endpoint, supportedProtocolVersion: 4);

        Assert.Equal("RS485-A", transport.LastBusId);
        Assert.Equal((byte)10, transport.LastUnitAddress);
        Assert.Equal((ushort)0, transport.LastStartAddress);
        Assert.Equal((ushort)B0Identification.RegisterCount, transport.LastRegisterCount);
        Assert.Equal(TR2SessionState.Compatible, session.State);
        Assert.Equal(new DeviceId(42), session.Device!.DeviceId);
    }

    [Fact]
    public async Task Reader_propagates_transport_failure_without_inventing_session()
    {
        var transport = new FakeRegisterTransport
        {
            Failure = new IOException("transport unavailable")
        };
        var reader = new B0Reader(transport);

        var exception = await Assert.ThrowsAsync<IOException>(async () =>
            await reader.ReadSessionAsync(Endpoint("RS485-A", 10), supportedProtocolVersion: 4));

        Assert.Equal("transport unavailable", exception.Message);
    }

    [Fact]
    public async Task Reader_rejects_short_B0_response_through_existing_parser_rule()
    {
        var transport = new FakeRegisterTransport
        {
            Registers = new ushort[B0Identification.RegisterCount - 1]
        };
        var reader = new B0Reader(transport);

        await Assert.ThrowsAsync<ArgumentException>(async () =>
            await reader.ReadSessionAsync(Endpoint("RS485-A", 10), supportedProtocolVersion: 4));
    }

    private static TR2Endpoint Endpoint(string bus, byte address) =>
        new(new SerialBus(bus), new ModbusAddress(address));

    private sealed class FakeRegisterTransport : IRegisterTransport
    {
        public ushort[] Registers { get; set; } = new ushort[B0Identification.RegisterCount];
        public Exception? Failure { get; init; }
        public string? LastBusId { get; private set; }
        public byte LastUnitAddress { get; private set; }
        public ushort LastStartAddress { get; private set; }
        public ushort LastRegisterCount { get; private set; }

        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default)
        {
            LastBusId = busId;
            LastUnitAddress = unitAddress;
            LastStartAddress = startAddress;
            LastRegisterCount = registerCount;

            if (Failure is not null)
            {
                return ValueTask.FromException<ushort[]>(Failure);
            }

            return ValueTask.FromResult(Registers);
        }
    }
}
