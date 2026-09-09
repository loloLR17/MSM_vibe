using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Protocol.Tests;

public sealed class B6CampaignSelectorTests
{
    [Fact]
    public async Task Selector_writes_exact_selected_campaign_index_register()
    {
        var transport = new RecordingWriteTransport();
        var selector = new B6CampaignSelector(transport);
        var endpoint = Endpoint();

        await selector.SelectAsync(endpoint, 7);

        Assert.Equal("RS485-A", transport.BusId);
        Assert.Equal((byte)10, transport.UnitAddress);
        Assert.Equal(B6CampaignSelector.SelectedCampaignIndexAddress, transport.StartAddress);
        Assert.Equal(new ushort[] { 7 }, transport.Values);
    }

    [Fact]
    public async Task Out_of_range_semantic_index_is_not_filtered_by_supervision()
    {
        var transport = new RecordingWriteTransport();
        var selector = new B6CampaignSelector(transport);

        await selector.SelectAsync(Endpoint(), ushort.MaxValue);

        Assert.Equal(new ushort[] { ushort.MaxValue }, transport.Values);
    }

    [Fact]
    public async Task Selector_propagates_transport_failure()
    {
        var expected = new IOException("Injected B6 selection failure.");
        var selector = new B6CampaignSelector(new ThrowingWriteTransport(expected));

        var actual = await Assert.ThrowsAsync<IOException>(async () =>
            await selector.SelectAsync(Endpoint(), 3));

        Assert.Same(expected, actual);
    }

    private static TR2Endpoint Endpoint() =>
        new(new SerialBus("RS485-A"), new ModbusAddress(10));

    private sealed class RecordingWriteTransport : IRegisterWriteTransport
    {
        public string? BusId { get; private set; }
        public byte? UnitAddress { get; private set; }
        public ushort? StartAddress { get; private set; }
        public ushort[]? Values { get; private set; }

        public ValueTask WriteRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            IReadOnlyList<ushort> values,
            CancellationToken cancellationToken = default)
        {
            BusId = busId;
            UnitAddress = unitAddress;
            StartAddress = startAddress;
            Values = values.ToArray();
            return ValueTask.CompletedTask;
        }
    }

    private sealed class ThrowingWriteTransport : IRegisterWriteTransport
    {
        private readonly Exception _exception;

        public ThrowingWriteTransport(Exception exception)
        {
            _exception = exception;
        }

        public ValueTask WriteRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            IReadOnlyList<ushort> values,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromException(_exception);
    }
}
