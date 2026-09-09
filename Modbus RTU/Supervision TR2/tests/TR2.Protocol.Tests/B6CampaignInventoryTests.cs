using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Protocol.Tests;

public sealed class B6CampaignInventoryTests
{
    [Fact]
    public void Parser_decodes_normative_B6_fields_and_msw_lsw_values()
    {
        var registers = new ushort[B6CampaignInventoryState.RegisterCount];
        registers[0] = 1;
        registers[1] = 5;
        registers[2] = 4;
        registers[3] = 2;
        registers[4] = 1;
        registers[5] = 0x0001;
        registers[6] = 0x0002;
        registers[7] = 0x0003;
        registers[8] = 0x0004;
        registers[9] = 1;
        registers[12] = 0x0005;
        registers[13] = 0x0006;
        registers[14] = 0x0007;
        registers[15] = 0x0008;
        registers[16] = 0x0009;
        registers[17] = 0x000A;
        registers[18] = 0x000B;
        registers[19] = 0x000C;
        registers[20] = 3;
        registers[21] = 0x000D;
        registers[22] = 0x000E;
        registers[23] = 0x000F;
        registers[24] = 0x0010;
        EncodeAscii(registers, 25, "CAMPAIGN-A");
        EncodeAscii(registers, 41, "MISSION-B");
        registers[57] = 1;

        var state = B6CampaignInventoryState.Parse(registers);

        Assert.Equal((ushort)1, state.InventoryStructureVersion);
        Assert.Equal((ushort)5, state.TotalCampaignCount);
        Assert.Equal((ushort)4, state.ValidCampaignCount);
        Assert.Equal((ushort)2, state.SelectedCampaignIndex);
        Assert.Equal((ushort)1, state.SelectedCampaignValid);
        Assert.Equal((uint)0x00010002, state.StorageUsedMb);
        Assert.Equal((uint)0x00030004, state.StorageFreeMb);
        Assert.Equal((ushort)1, state.StorageHealthStatus);
        Assert.Equal((uint)0x00050006, state.CampaignId);
        Assert.Equal((uint)0x00070008, state.MissionId);
        Assert.Equal((uint)0x0009000A, state.StartTimestampSeconds);
        Assert.Equal((uint)0x000B000C, state.EndTimestampSeconds);
        Assert.Equal((ushort)3, state.CampaignState);
        Assert.Equal((uint)0x000D000E, state.DurationSeconds);
        Assert.Equal((uint)0x000F0010, state.DataSizeMb);
        Assert.Equal("CAMPAIGN-A", state.CampaignLabel);
        Assert.Equal("MISSION-B", state.MissionLabel);
        Assert.Equal((ushort)1, state.DataIntegrityStatus);
    }

    [Fact]
    public void Parser_requires_complete_B6_register_image()
    {
        var incomplete = new ushort[B6CampaignInventoryState.RegisterCount - 1];

        Assert.Throws<ArgumentException>(() => B6CampaignInventoryState.Parse(incomplete));
    }

    [Fact]
    public async Task Reader_requests_exact_B6_range()
    {
        var transport = new RecordingTransport(new ushort[B6CampaignInventoryState.RegisterCount]);
        var reader = new B6Reader(transport);
        var endpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(10));

        await reader.ReadAsync(endpoint);

        Assert.Equal("RS485-A", transport.BusId);
        Assert.Equal((byte)10, transport.UnitAddress);
        Assert.Equal(B6CampaignInventoryState.StartAddress, transport.StartAddress);
        Assert.Equal(B6CampaignInventoryState.RegisterCount, transport.RegisterCount);
    }

    [Fact]
    public async Task Reader_propagates_transport_failure()
    {
        var expected = new InvalidOperationException("transport failed");
        var reader = new B6Reader(new ThrowingTransport(expected));
        var endpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(10));

        var actual = await Assert.ThrowsAsync<InvalidOperationException>(async () =>
            await reader.ReadAsync(endpoint));

        Assert.Same(expected, actual);
    }

    private static void EncodeAscii(ushort[] registers, int startOffset, string value)
    {
        for (var i = 0; i < value.Length; i += 2)
        {
            var high = (byte)value[i];
            var low = i + 1 < value.Length ? (byte)value[i + 1] : (byte)0;
            registers[startOffset + (i / 2)] = (ushort)((high << 8) | low);
        }
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
