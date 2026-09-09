using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Protocol.Tests;

public sealed class B5CommandStateTests
{
    [Fact]
    public void Parse_decodes_complete_V1_projection_and_uint32_fields()
    {
        ushort[] registers =
        [
            10, 42, 1, 2,
            0x1234, 0x5678,
            0xA55A, 0x0001,
            10, 42, 3, 0, 7, 0x0005,
            9, 41, 4, 0,
            0x0102, 0x0304
        ];

        var state = B5CommandState.Parse(registers);

        Assert.Equal((ushort)10, state.RequestCode);
        Assert.Equal((ushort)42, state.RequestTransactionId);
        Assert.Equal(0x12345678u, state.RequestParam3);
        Assert.Equal((ushort)10, state.ActiveCode);
        Assert.Equal((ushort)42, state.ActiveTransactionId);
        Assert.Equal((ushort)3, state.Status);
        Assert.Equal((ushort)7, state.ResultDetail);
        Assert.Equal((ushort)9, state.LastCode);
        Assert.Equal((ushort)41, state.LastTransactionId);
        Assert.Equal(0x01020304u, state.LastTimestampSeconds);
    }

    [Fact]
    public void Parse_requires_complete_twenty_register_image()
    {
        var registers = new ushort[B5CommandState.RegisterCount - 1];

        Assert.Throws<ArgumentException>(() => B5CommandState.Parse(registers));
    }

    [Fact]
    public async Task Reader_requests_exact_B5_range()
    {
        var transport = new RecordingTransport(new ushort[B5CommandState.RegisterCount]);
        var reader = new B5Reader(transport);
        var endpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(17));

        await reader.ReadAsync(endpoint);

        Assert.Equal("RS485-A", transport.BusId);
        Assert.Equal((byte)17, transport.UnitAddress);
        Assert.Equal(B5CommandState.StartAddress, transport.StartAddress);
        Assert.Equal(B5CommandState.RegisterCount, transport.RegisterCount);
    }

    [Fact]
    public async Task Reader_propagates_transport_failure()
    {
        var reader = new B5Reader(new FailingTransport());
        var endpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(17));

        await Assert.ThrowsAsync<IOException>(async () => await reader.ReadAsync(endpoint));
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

    private sealed class FailingTransport : IRegisterTransport
    {
        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromException<ushort[]>(new IOException("Injected transport failure."));
    }
}
