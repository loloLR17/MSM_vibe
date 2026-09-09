using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Protocol.Tests;

public sealed class B5CommandWriterTests
{
    private static readonly TR2Endpoint Endpoint =
        new(new SerialBus("RS485-A"), new ModbusAddress(10));

    [Fact]
    public async Task Writes_complete_request_with_control_zero_before_submit_edge()
    {
        var transport = new RecordingWriteTransport();
        var writer = new B5CommandWriter(transport);
        var request = new B5CommandRequest(
            Code: 3,
            TransactionId: 42,
            Param1: 11,
            Param2: 22,
            Param3: 0x12345678,
            ConfirmKey: 0xA55A);

        await writer.WriteAndSubmitAsync(Endpoint, request);

        Assert.Equal(2, transport.Calls.Count);

        var prepare = transport.Calls[0];
        Assert.Equal("RS485-A", prepare.BusId);
        Assert.Equal((byte)10, prepare.UnitAddress);
        Assert.Equal((ushort)5000, prepare.StartAddress);
        Assert.Equal(
            new ushort[] { 3, 42, 11, 22, 0x1234, 0x5678, 0xA55A, 0 },
            prepare.Values);

        var submit = transport.Calls[1];
        Assert.Equal((ushort)5007, submit.StartAddress);
        Assert.Equal(new ushort[] { 0x0001 }, submit.Values);
    }

    [Fact]
    public async Task Preparation_failure_prevents_submit_write()
    {
        var transport = new RecordingWriteTransport { FailOnCall = 1 };
        var writer = new B5CommandWriter(transport);

        await Assert.ThrowsAsync<IOException>(async () =>
            await writer.WriteAndSubmitAsync(
                Endpoint,
                new B5CommandRequest(3, 42, 0, 0, 0, 0)));

        Assert.Single(transport.Calls);
    }

    [Fact]
    public async Task Submit_failure_occurs_only_after_prepared_request_was_written()
    {
        var transport = new RecordingWriteTransport { FailOnCall = 2 };
        var writer = new B5CommandWriter(transport);

        await Assert.ThrowsAsync<IOException>(async () =>
            await writer.WriteAndSubmitAsync(
                Endpoint,
                new B5CommandRequest(3, 42, 0, 0, 0, 0)));

        Assert.Equal(2, transport.Calls.Count);
        Assert.Equal((ushort)5000, transport.Calls[0].StartAddress);
        Assert.Equal((ushort)5007, transport.Calls[1].StartAddress);
    }

    [Fact]
    public void Prepared_registers_encode_param3_msw_then_lsw_and_control_zero()
    {
        var request = new B5CommandRequest(10, 65535, 1, 2, 0x89ABCDEF, 0xA55A);

        Assert.Equal(
            new ushort[] { 10, 65535, 1, 2, 0x89AB, 0xCDEF, 0xA55A, 0 },
            request.ToPreparedRegisters());
    }

    private sealed record WriteCall(
        string BusId,
        byte UnitAddress,
        ushort StartAddress,
        ushort[] Values);

    private sealed class RecordingWriteTransport : IRegisterWriteTransport
    {
        public List<WriteCall> Calls { get; } = [];
        public int? FailOnCall { get; init; }

        public ValueTask WriteRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            IReadOnlyList<ushort> values,
            CancellationToken cancellationToken = default)
        {
            Calls.Add(new WriteCall(busId, unitAddress, startAddress, values.ToArray()));

            if (FailOnCall == Calls.Count)
            {
                throw new IOException("Injected write failure.");
            }

            return ValueTask.CompletedTask;
        }
    }
}
