using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Protocol.Tests;

public sealed class B7DiagnosticStateTests
{
    [Fact]
    public void Parser_decodes_normative_B7_fields_including_signed_temperature_and_uint32()
    {
        var registers = new ushort[B7DiagnosticState.RegisterCount];
        registers[0] = 1;
        registers[1] = 2;
        registers[2] = 0x0105;
        registers[3] = 42;
        registers[4] = 0x1234;
        registers[5] = 0x5678;
        registers[6] = 3;
        registers[7] = 7;
        registers[8] = 9;
        registers[9] = 0x89AB;
        registers[10] = 0xCDEF;
        registers[11] = 3;
        registers[12] = unchecked((ushort)-50);
        registers[13] = 3300;

        var state = B7DiagnosticState.Parse(registers);

        Assert.Equal((ushort)1, state.DiagnosticStructureVersion);
        Assert.Equal((ushort)2, state.SystemHealthStatus);
        Assert.Equal((ushort)0x0105, state.SystemFaultFlags);
        Assert.Equal((ushort)42, state.LastFaultCode);
        Assert.Equal((uint)0x12345678, state.LastFaultTimestampSeconds);
        Assert.Equal((ushort)3, state.SelftestStatus);
        Assert.Equal((ushort)7, state.SelftestResultCode);
        Assert.Equal((ushort)9, state.SelftestDetail);
        Assert.Equal((uint)0x89ABCDEF, state.UptimeSeconds);
        Assert.Equal((ushort)3, state.ResetCause);
        Assert.Equal((short)-50, state.InternalTemperatureDeciCelsius);
        Assert.Equal((ushort)3300, state.SupplyVoltageMillivolts);
    }

    [Fact]
    public void Parser_requires_complete_B7_register_image()
    {
        var incomplete = new ushort[B7DiagnosticState.RegisterCount - 1];

        Assert.Throws<ArgumentException>(() => B7DiagnosticState.Parse(incomplete));
    }

    [Fact]
    public async Task Reader_requests_exact_B7_range()
    {
        var transport = new RecordingTransport(new ushort[B7DiagnosticState.RegisterCount]);
        var reader = new B7Reader(transport);
        var endpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(10));

        await reader.ReadAsync(endpoint);

        Assert.Equal("RS485-A", transport.BusId);
        Assert.Equal((byte)10, transport.UnitAddress);
        Assert.Equal(B7DiagnosticState.StartAddress, transport.StartAddress);
        Assert.Equal(B7DiagnosticState.RegisterCount, transport.RegisterCount);
    }

    [Fact]
    public async Task Reader_propagates_transport_failure()
    {
        var expected = new IOException("transport failed");
        var reader = new B7Reader(new ThrowingTransport(expected));
        var endpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(10));

        var actual = await Assert.ThrowsAsync<IOException>(async () =>
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
