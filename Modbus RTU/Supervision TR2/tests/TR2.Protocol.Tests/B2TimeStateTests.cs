using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Protocol.Tests;

public sealed class B2TimeStateTests
{
    [Fact]
    public void Parser_decodes_normative_B2_fields_and_signed_drift()
    {
        var registers = new ushort[B2TimeState.RegisterCount];
        registers[0] = 3;
        registers[1] = 0x0093;
        registers[2] = 0x1234;
        registers[3] = 0x5678;
        registers[4] = 0x0001;
        registers[5] = 0x0020;
        registers[6] = 0x0000;
        registers[7] = 0x003C;
        registers[8] = 0xABCD;
        registers[9] = 0xEF01;
        registers[10] = 1;
        registers[11] = 25;
        registers[12] = unchecked((ushort)-17);
        registers[13] = 2;

        var state = B2TimeState.Parse(registers);

        Assert.Equal((ushort)3, state.TimeStatus);
        Assert.Equal((ushort)0x0093, state.TimeFlags);
        Assert.Equal((uint)0x12345678, state.CurrentTimeSeconds);
        Assert.Equal((uint)0x00010020, state.LastSyncTimeSeconds);
        Assert.Equal((uint)60, state.TimeSinceSyncSeconds);
        Assert.Equal((uint)0xABCDEF01, state.PreparedTimeSeconds);
        Assert.Equal((ushort)1, state.PreparedTimeStatus);
        Assert.Equal((ushort)25, state.TimeAccuracyMilliseconds);
        Assert.Equal((short)-17, state.DriftPpm);
        Assert.Equal((ushort)2, state.SyncSource);
    }

    [Fact]
    public void Parser_requires_complete_B2_register_image()
    {
        var incomplete = new ushort[B2TimeState.RegisterCount - 1];

        Assert.Throws<ArgumentException>(() => B2TimeState.Parse(incomplete));
    }

    [Fact]
    public async Task Reader_requests_exact_B2_range()
    {
        var transport = new RecordingTransport(new ushort[B2TimeState.RegisterCount]);
        var reader = new B2Reader(transport);
        var endpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(10));

        await reader.ReadAsync(endpoint);

        Assert.Equal("RS485-A", transport.BusId);
        Assert.Equal((byte)10, transport.UnitAddress);
        Assert.Equal(B2TimeState.StartAddress, transport.StartAddress);
        Assert.Equal(B2TimeState.RegisterCount, transport.RegisterCount);
    }

    [Fact]
    public async Task Reader_propagates_transport_failure()
    {
        var expected = new InvalidOperationException("transport failed");
        var reader = new B2Reader(new ThrowingTransport(expected));
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
