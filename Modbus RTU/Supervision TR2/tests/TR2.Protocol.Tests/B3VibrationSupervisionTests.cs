using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Protocol.Tests;

public sealed class B3VibrationSupervisionTests
{
    [Fact]
    public void Parser_decodes_normative_B3_fields_and_msw_lsw_values()
    {
        var registers = new ushort[B3VibrationSupervision.RegisterCount];
        registers[0] = 3;
        registers[1] = 0x020F;
        registers[2] = 0x0011;
        registers[3] = 4;
        registers[4] = 0x0001;
        registers[5] = 0x0002;
        registers[6] = 0x0003;
        registers[7] = 0x0004;
        registers[8] = 0x0005;
        registers[9] = 0x0006;
        registers[10] = 0x0007;
        registers[11] = 0x0008;
        registers[12] = 0x0009;
        registers[13] = 0x000A;
        registers[14] = 0x000B;
        registers[15] = 0x000C;
        registers[16] = 0x000D;
        registers[17] = 0x000E;
        registers[18] = 0x000F;
        registers[19] = 0x0010;
        registers[20] = 0x0011;
        registers[21] = 0x0012;
        registers[22] = 0x0013;
        registers[23] = 0x0014;
        registers[24] = 0x0015;
        registers[25] = 0x0016;
        registers[26] = 0x0017;
        registers[27] = 0x0018;
        registers[28] = 0x0019;
        registers[29] = 0x001A;
        registers[30] = 2;
        registers[31] = 1;
        registers[32] = 0;
        registers[33] = 1;
        registers[34] = 0;
        registers[35] = 1;
        registers[36] = 0x001B;
        registers[37] = 0x001C;
        registers[38] = 0x001D;
        registers[39] = 0x001E;

        var state = B3VibrationSupervision.Parse(registers);

        Assert.Equal((ushort)3, state.StatusGlobal);
        Assert.Equal((ushort)0x020F, state.ValidityFlags);
        Assert.Equal((uint)0x00010002, state.LastUpdateTr2Seconds);
        Assert.Equal((uint)0x00030004, state.ValueAgeMilliseconds);
        Assert.Equal((uint)0x000B000C, state.RmsGlobalMg);
        Assert.Equal((uint)0x000D000E, state.PeakGlobalMg);
        Assert.Equal((uint)0x0019001A, state.PeakZMg);
        Assert.Equal((ushort)2, state.DominantAxis);
        Assert.Equal((ushort)1, state.AlarmLatched);
        Assert.Equal((uint)0x001B001C, state.ExceedCount);
        Assert.Equal((uint)0x001D001E, state.AlarmCount);
    }

    [Fact]
    public void Parser_requires_complete_B3_register_image()
    {
        var incomplete = new ushort[B3VibrationSupervision.RegisterCount - 1];

        Assert.Throws<ArgumentException>(() => B3VibrationSupervision.Parse(incomplete));
    }

    [Fact]
    public async Task Reader_requests_exact_B3_range()
    {
        var transport = new RecordingTransport(new ushort[B3VibrationSupervision.RegisterCount]);
        var reader = new B3Reader(transport);
        var endpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(10));

        await reader.ReadAsync(endpoint);

        Assert.Equal("RS485-A", transport.BusId);
        Assert.Equal((byte)10, transport.UnitAddress);
        Assert.Equal(B3VibrationSupervision.StartAddress, transport.StartAddress);
        Assert.Equal(B3VibrationSupervision.RegisterCount, transport.RegisterCount);
    }

    [Fact]
    public async Task Reader_propagates_transport_failure()
    {
        var expected = new InvalidOperationException("transport failed");
        var reader = new B3Reader(new ThrowingTransport(expected));
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
