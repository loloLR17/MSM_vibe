using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Protocol.Tests;

public sealed class B1SystemStateTests
{
    [Fact]
    public void Parser_decodes_normative_B1_fields_and_msw_lsw_values()
    {
        var registers = new ushort[B1SystemState.RegisterCount];
        registers[0] = 2;
        registers[1] = 0x0015;
        registers[2] = 0x0004;
        registers[3] = 0x0002;
        registers[4] = 0x1234;
        registers[5] = 0xABCD;
        registers[6] = 3;
        registers[7] = unchecked((ushort)-50);
        registers[8] = 42;
        registers[9] = 51;
        registers[10] = 1;
        registers[11] = 75;
        registers[12] = 1;
        registers[13] = 0x0001;
        registers[14] = 0x002A;
        registers[15] = 7;
        registers[16] = 8;

        var state = B1SystemState.Parse(registers);

        Assert.Equal((ushort)2, state.SystemStatus);
        Assert.Equal((ushort)0x0015, state.SystemFlags);
        Assert.Equal((uint)0x1234ABCD, state.UptimeSeconds);
        Assert.Equal((short)-50, state.InternalTemperatureDeciCelsius);
        Assert.Equal((uint)0x0001002A, state.ActiveCampaignId);
        Assert.Equal((ushort)1, state.AcquisitionState);
        Assert.Equal((ushort)7, state.ErrorCode);
        Assert.Equal((ushort)8, state.WarningCode);
    }

    [Fact]
    public void Parser_requires_complete_B1_register_image()
    {
        var incomplete = new ushort[B1SystemState.RegisterCount - 1];

        Assert.Throws<ArgumentException>(() => B1SystemState.Parse(incomplete));
    }

    [Fact]
    public async Task Reader_requests_exact_B1_range()
    {
        var transport = new RecordingTransport(new ushort[B1SystemState.RegisterCount]);
        var reader = new B1Reader(transport);
        var endpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(10));

        await reader.ReadAsync(endpoint);

        Assert.Equal("RS485-A", transport.BusId);
        Assert.Equal((byte)10, transport.UnitAddress);
        Assert.Equal(B1SystemState.StartAddress, transport.StartAddress);
        Assert.Equal((ushort)B1SystemState.RegisterCount, transport.RegisterCount);
    }

    [Fact]
    public async Task Reader_propagates_transport_failure()
    {
        var expected = new InvalidOperationException("transport failed");
        var reader = new B1Reader(new ThrowingTransport(expected));
        var endpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(10));

        var actual = await Assert.ThrowsAsync<InvalidOperationException>(async () =>
            await reader.ReadAsync(endpoint));

        Assert.Same(expected, actual);
    }

    private sealed class RecordingTransport : IRegisterTransport
    {
        private readonly ushort[] _registers;

        public RecordingTransport(ushort[] registers)
        {
            _registers = registers;
        }

        public string? BusId { get; private set; }
        public byte? UnitAddress { get; private set; }
        public ushort? StartAddress { get; private set; }
        public ushort? RegisterCount { get; private set; }

        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default)
        {
            BusId = busId;
            UnitAddress = unitAddress;
            StartAddress = startAddress;
            RegisterCount = registerCount;
            return ValueTask.FromResult(_registers);
        }
    }

    private sealed class ThrowingTransport : IRegisterTransport
    {
        private readonly Exception _exception;

        public ThrowingTransport(Exception exception)
        {
            _exception = exception;
        }

        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromException<ushort[]>(_exception);
    }
}
