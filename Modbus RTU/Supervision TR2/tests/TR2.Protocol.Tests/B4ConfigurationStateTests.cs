using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Protocol.Tests;

public sealed class B4ConfigurationStateTests
{
    [Fact]
    public void Parser_decodes_normative_prepared_and_active_B4_images()
    {
        var registers = new ushort[B4ConfigurationState.RegisterCount];
        registers[0] = 1;
        registers[1] = 0x0003;
        SetUInt32(registers, 2, 0x01020304);
        SetUInt32(registers, 4, 0x11121314);
        registers[6] = 2;
        registers[7] = 0;
        SetUInt32(registers, 8, 0x5207CCFC);
        SetUInt32(registers, 10, 0x21222324);
        SetUInt32(registers, 12, 0x31323334);

        registers[16] = 26667;
        registers[18] = 0x0007;
        registers[19] = 2;
        registers[20] = 1;
        registers[21] = 32768;
        registers[22] = 5000;
        SetUInt32(registers, 23, 3600);
        registers[25] = 1;
        SetUInt32(registers, 26, 512);
        registers[40] = 1;
        registers[41] = 100;
        registers[42] = 200;
        registers[43] = 300;
        registers[44] = 400;
        registers[45] = 20;
        registers[46] = 500;
        SetUInt32(registers, 56, 1);
        SetUInt32(registers, 58, 2);
        SetAscii(registers, 60, "CAMPAIGN-A");
        SetAscii(registers, 76, "MISSION-B");
        registers[92] = 1;
        registers[93] = 2;
        registers[94] = 3;
        registers[95] = 4;

        registers[100] = 26667;
        registers[101] = 0x0003;
        registers[102] = 1;
        registers[103] = 1;
        registers[104] = 16384;
        registers[105] = 10000;
        SetUInt32(registers, 106, 7200);
        registers[108] = 1;
        SetUInt32(registers, 109, 1024);
        registers[116] = 1;
        registers[117] = 110;
        registers[118] = 210;
        registers[119] = 310;
        registers[120] = 410;
        registers[121] = 25;
        registers[122] = 600;
        SetUInt32(registers, 128, 3);
        SetUInt32(registers, 130, 4);
        SetAscii(registers, 132, "ACTIVE-CAMPAIGN");
        SetAscii(registers, 148, "ACTIVE-MISSION");
        registers[164] = 5;
        registers[165] = 6;
        registers[166] = 7;
        registers[167] = 8;

        var state = B4ConfigurationState.Parse(registers);

        Assert.Equal((uint)0x01020304, state.PreparedConfigId);
        Assert.Equal((uint)0x11121314, state.ActiveConfigId);
        Assert.Equal((uint)0x5207CCFC, state.PreparedConfigCrc);
        Assert.Equal((ushort)26667, state.SamplingFrequencyHz);
        Assert.Equal((uint)3600, state.CampaignDurationSeconds);
        Assert.Equal((uint)512, state.StorageLimitMb);
        Assert.Equal("CAMPAIGN-A", state.CampaignLabel);
        Assert.Equal("MISSION-B", state.MissionLabel);
        Assert.Equal((ushort)26667, state.ActiveSamplingFrequencyHz);
        Assert.Equal((uint)7200, state.ActiveCampaignDurationSeconds);
        Assert.Equal((uint)1024, state.ActiveStorageLimitMb);
        Assert.Equal("ACTIVE-CAMPAIGN", state.ActiveCampaignLabel);
        Assert.Equal("ACTIVE-MISSION", state.ActiveMissionLabel);
        Assert.Equal((ushort)8, state.ActiveSeaStateCode);
    }

    [Fact]
    public void Parser_requires_complete_B4_register_image()
    {
        var incomplete = new ushort[B4ConfigurationState.RegisterCount - 1];

        Assert.Throws<ArgumentException>(() => B4ConfigurationState.Parse(incomplete));
    }

    [Fact]
    public async Task Reader_requests_exact_normative_B4_range()
    {
        var transport = new RecordingTransport(new ushort[B4ConfigurationState.RegisterCount]);
        var reader = new B4Reader(transport);
        var endpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(10));

        await reader.ReadAsync(endpoint);

        Assert.Equal("RS485-A", transport.BusId);
        Assert.Equal((byte)10, transport.UnitAddress);
        Assert.Equal(B4ConfigurationState.StartAddress, transport.StartAddress);
        Assert.Equal(B4ConfigurationState.RegisterCount, transport.RegisterCount);
    }

    [Fact]
    public async Task Reader_propagates_transport_failure()
    {
        var expected = new InvalidOperationException("transport failed");
        var reader = new B4Reader(new ThrowingTransport(expected));
        var endpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(10));

        var actual = await Assert.ThrowsAsync<InvalidOperationException>(async () =>
            await reader.ReadAsync(endpoint));

        Assert.Same(expected, actual);
    }

    private static void SetUInt32(ushort[] registers, int offset, uint value)
    {
        registers[offset] = (ushort)(value >> 16);
        registers[offset + 1] = (ushort)value;
    }

    private static void SetAscii(ushort[] registers, int offset, string value)
    {
        for (var index = 0; index < value.Length && index < 32; index += 2)
        {
            var high = (ushort)(value[index] << 8);
            var low = index + 1 < value.Length ? value[index + 1] : 0;
            registers[offset + (index / 2)] = (ushort)(high | low);
        }
    }

    private sealed class RecordingTransport : IRegisterTransport
    {
        private readonly ushort[] _registers;

        public RecordingTransport(ushort[] registers) => _registers = registers;

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

        public ThrowingTransport(Exception exception) => _exception = exception;

        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromException<ushort[]>(_exception);
    }
}
