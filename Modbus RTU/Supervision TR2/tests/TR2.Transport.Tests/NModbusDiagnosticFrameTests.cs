using NModbus;
using NModbus.IO;
using Xunit;

namespace TR2.Transport.Tests;

public sealed class NModbusDiagnosticFrameTests
{
    [Fact]
    public async Task RealNModbusEmitsExactFc03FrameAndAcceptsUnknownWithValidCrc()
    {
        using var stream = new MemorySerialStream(Convert.FromHexString("0103020000B844"));
        using var master = new ModbusFactory().CreateRtuMaster(stream);
        NModbusSafetyPolicy.Apply(master);
        Assert.Equal(new ushort[] { 0 }, await master.ReadHoldingRegistersAsync(1, 1000, 1));
        Assert.Equal("010303E80001047A", Convert.ToHexString(Assert.Single(stream.Writes)));
        Assert.Equal(0, master.Transport.Retries);
        Assert.Equal(0, master.Transport.WaitToRetryMilliseconds);
        Assert.True(master.Transport.SlaveBusyUsesRetryCount);
    }

    [Theory]
    [InlineData("0103020000B845", typeof(IOException))] // bad CRC
    [InlineData("01030400000000FA33", typeof(IOException))] // two registers, valid CRC
    [InlineData("0203020000FC44", typeof(IOException))] // wrong unit, valid CRC
    [InlineData("0104020000B930", typeof(IOException))] // wrong function, valid CRC
    [InlineData("0103020000", typeof(TimeoutException))] // truncated reply
    [InlineData("01830440F3", typeof(SlaveException))] // unavailable (04), valid CRC
    [InlineData("018306C132", typeof(SlaveException))] // busy (06), no resend
    [InlineData("", typeof(TimeoutException))]
    public async Task RealNModbusRejectsFailedFramesWithoutRetransmission(string reply, Type exceptionType)
    {
        using var stream = new MemorySerialStream(Convert.FromHexString(reply));
        using var master = new ModbusFactory().CreateRtuMaster(stream);
        NModbusSafetyPolicy.Apply(master);
        var exception = await Record.ExceptionAsync(() => master.ReadHoldingRegistersAsync(1, 1000, 1));
        Assert.IsType(exceptionType, exception);
        Assert.Equal("010303E80001047A", Convert.ToHexString(Assert.Single(stream.Writes)));
    }

    private sealed class MemorySerialStream(byte[] response) : IStreamResource
    {
        private int _offset;
        public List<byte[]> Writes { get; } = [];
        public int InfiniteTimeout => -1;
        public int ReadTimeout { get; set; } = 100;
        public int WriteTimeout { get; set; } = 100;
        public void DiscardInBuffer() { } // response becomes available only after request write
        public int Read(byte[] buffer, int offset, int count)
        {
            Assert.Single(Writes);
            var available = Math.Min(count, response.Length - _offset);
            if (available == 0) { throw new TimeoutException("simulated silence"); }
            Array.Copy(response, _offset, buffer, offset, available);
            _offset += available;
            return available;
        }
        public void Write(byte[] buffer, int offset, int count) => Writes.Add(buffer.AsSpan(offset, count).ToArray());
        public void Dispose() { }
    }
}
