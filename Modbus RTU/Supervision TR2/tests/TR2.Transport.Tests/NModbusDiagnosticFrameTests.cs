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

    [Theory]
    [InlineData(false)]
    [InlineData(true)]
    public async Task SystemBlocksUseExactB0B1RequestsAndNeverRetryFailedB1(bool corruptB1)
    {
        var b0 = new ushort[21];
        b0[0] = 0x1234; b0[1] = 0x5678; b0[6] = 1;
        var b1 = new ushort[20];
        b1[0] = 0; b1[7] = unchecked((ushort)-123);
        using var stream = new ScriptedBlockStream([Reply(b0), Reply(b1)]);
        if (corruptB1) stream.Responses[1][^1] ^= 1;
        using var master = new ModbusFactory().CreateRtuMaster(stream);
        NModbusSafetyPolicy.Apply(master);
        Assert.Equal(b0, await master.ReadHoldingRegistersAsync(1, 0, 21));
        if (corruptB1)
            await Assert.ThrowsAsync<IOException>(() => master.ReadHoldingRegistersAsync(1, 1000, 20));
        else
            Assert.Equal(b1, await master.ReadHoldingRegistersAsync(1, 1000, 20));
        Assert.Equal(new[] { "0103000000158405", "010303E80014C5B5" }, stream.Writes.Select(Convert.ToHexString));
    }

    private static byte[] Reply(ushort[] registers)
    {
        var bytes = new byte[5 + registers.Length * 2];
        bytes[0] = 1; bytes[1] = 3; bytes[2] = (byte)(registers.Length * 2);
        for (var i = 0; i < registers.Length; i++)
        {
            bytes[3 + 2 * i] = (byte)(registers[i] >> 8);
            bytes[4 + 2 * i] = (byte)registers[i];
        }
        ushort crc = 0xffff;
        foreach (var b in bytes.AsSpan(0, bytes.Length - 2))
        {
            crc ^= b;
            for (var i = 0; i < 8; i++) crc = (ushort)((crc >> 1) ^ ((crc & 1) != 0 ? 0xa001 : 0));
        }
        bytes[^2] = (byte)crc; bytes[^1] = (byte)(crc >> 8);
        return bytes;
    }

    private sealed class ScriptedBlockStream(byte[][] responses) : IStreamResource
    {
        private int _offset;
        public byte[][] Responses { get; } = responses;
        public List<byte[]> Writes { get; } = [];
        public int InfiniteTimeout => -1;
        public int ReadTimeout { get; set; } = 100;
        public int WriteTimeout { get; set; } = 100;
        public void DiscardInBuffer() { }
        public int Read(byte[] buffer, int offset, int count)
        {
            var response = Responses[Writes.Count - 1];
            var available = Math.Min(count, response.Length - _offset);
            if (available == 0) throw new TimeoutException("simulated block silence");
            Array.Copy(response, _offset, buffer, offset, available);
            _offset += available;
            return available;
        }
        public void Write(byte[] buffer, int offset, int count)
        {
            Writes.Add(buffer.AsSpan(offset, count).ToArray());
            Assert.InRange(Writes.Count, 1, Responses.Length);
            _offset = 0;
        }
        public void Dispose() { }
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
