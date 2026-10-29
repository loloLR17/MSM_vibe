using NModbus.IO;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class LiveDirectTests
{
    [Fact]
    public async Task Ac1UsesExactExplicitReadonlyOptInAndRetainsAllOtherRangeGuards()
    {
        var factory=new Factory();
        await using(var old=new LiveDirectTransport(factory,"test-only"))
            await Assert.ThrowsAsync<InvalidDataException>(async()=>await old.ReadRegistersAsync("live-direct",1,8000,64));
        Assert.Equal(0,factory.Opens);
        await using(var enabled=new LiveDirectTransport(factory,"test-only",ac1Enabled:true))
        {
            var r=await enabled.ReadRegistersAsync("live-direct",1,8000,64);Assert.Equal(64,r.Length);
            await Assert.ThrowsAsync<InvalidDataException>(async()=>await enabled.ReadRegistersAsync("live-direct",1,8000,65));
            await Assert.ThrowsAsync<InvalidDataException>(async()=>await enabled.ReadRegistersAsync("other",1,8000,64));
            await Assert.ThrowsAsync<InvalidDataException>(async()=>await enabled.ReadRegistersAsync("live-direct",1,8001,63));
            Assert.Equal(0,factory.WriterAccesses);
        }
    }

    [Fact]
    public async Task ConcurrentReadsOpenOneExclusiveConnectionAndNeverObtainWriter()
    {
        var factory = new Factory();
        await using var transport = new LiveDirectTransport(factory, "test-only");
        await Task.WhenAll(Enumerable.Range(0, 20).Select(async _ => await transport.ReadRegistersAsync("live-direct", 1, 0, 21)));
        Assert.Equal(1, factory.Opens);
        Assert.Equal(1, factory.Peak);
        Assert.Equal(20, factory.Reads);
        Assert.Equal(0, factory.WriterAccesses);
        await transport.DisposeAsync();
        Assert.Equal(0, factory.Active);
        await Assert.ThrowsAsync<ObjectDisposedException>(async () => await transport.ReadRegistersAsync("live-direct", 1, 0, 21));
    }

    [Fact]
    public async Task LogicalDisconnectClosesHandleRejectsReadAndReconnectsWithoutOldValues()
    {
        var factory = new Factory();
        await using var transport = new LiveDirectTransport(factory, "test-only");
        await transport.ReadRegistersAsync("live-direct", 1, 0, 21);
        await transport.SetConnectedAsync(false);
        Assert.Equal(0, factory.Active);
        await Assert.ThrowsAsync<IOException>(async () => await transport.ReadRegistersAsync("live-direct", 1, 3000, 48));
        Assert.Equal(1, factory.Reads);
        await transport.SetConnectedAsync(true);
        await transport.ReadRegistersAsync("live-direct", 1, 3000, 48);
        Assert.Equal(2, factory.Opens);
        Assert.Equal(1, factory.Peak);
        Assert.Equal(0, factory.WriterAccesses);
    }

    [Fact]
    public async Task ReadFailureClosesHandleNoSameCycleRetryAndNextReadReopens()
    {
        var factory = new Factory { FailRead = true };
        await using var transport = new LiveDirectTransport(factory, "test-only");
        await Assert.ThrowsAsync<IOException>(async () => await transport.ReadRegistersAsync("live-direct", 1, 3000, 48));
        Assert.Equal(1, factory.Reads);
        Assert.Equal(0, factory.Active);
        factory.FailRead = false;
        await transport.ReadRegistersAsync("live-direct", 1, 3000, 48);
        Assert.Equal(2, factory.Opens);
    }

    [Theory]
    [InlineData("other", 0, 21)]
    [InlineData("live-direct", 5000, 29)]
    [InlineData("live-direct", 3000, 47)]
    public async Task BoundedReadRouteRejectsUnexpectedBusOrRegistersBeforeOpening(string bus, ushort address, ushort count)
    {
        var factory = new Factory();
        await using var transport = new LiveDirectTransport(factory, "test-only");
        await Assert.ThrowsAsync<InvalidDataException>(async () => await transport.ReadRegistersAsync(bus, 1, address, count));
        Assert.Equal(0, factory.Opens);
    }

    [Fact]
    public void RawTracePreservesActualChunksOffsetsTimeoutsAndDisposal()
    {
        var inner = new Stream(); var trace = new List<SerialStreamTrace>();
        using (var stream = new TracedStreamResource(inner, trace.Add))
        {
            stream.ReadTimeout = 120; stream.WriteTimeout = 130;
            var bytes = new byte[] { 9, 1, 3, 8 };
            stream.Write(bytes, 1, 2);
            Assert.Equal(2, stream.Read(bytes, 1, 3));
            Assert.Equal("0103", trace[0].Hex); Assert.Equal("tx", trace[0].Direction);
            Assert.Equal("AABB", trace[1].Hex); Assert.Equal("rx", trace[1].Direction);
            Assert.Equal(120, inner.ReadTimeout); Assert.Equal(130, inner.WriteTimeout);
        }
        Assert.True(inner.Disposed);
    }

    [Fact]
    public async Task MissingExplicitDiagnosticOptInFailsBeforeAnySerialOpen()
    {
        using var output = new StringWriter(); using var error = new StringWriter();
        Assert.Equal(2, await LiveDirectConsoleApplication.RunAsync(["--live-direct", "nonexistent"], default, output, error));
        Assert.Contains("--allow-diagnostic", error.ToString());
    }

    private sealed class Factory : IModbusBusConnectionFactory
    {
        public int Opens, Active, Peak, Reads, WriterAccesses;
        public bool FailRead;
        public ValueTask<IModbusBusConnection> OpenAsync(string id, ModbusSerialConnectionSettings settings, CancellationToken ct = default)
        {
            Opens++; Active++; Peak = Math.Max(Peak, Active);
            Assert.Equal(115200, settings.BaudRate); Assert.Equal(ModbusSerialParity.Even, settings.Parity);
            return ValueTask.FromResult<IModbusBusConnection>(new Connection(this, id));
        }
        private sealed class Connection(Factory owner, string id) : IModbusBusConnection, IRegisterTransport
        {
            private bool _disposed;
            public string BusId => id;
            public IRegisterTransport RegisterTransport => this;
            public IRegisterWriteTransport RegisterWriteTransport { get { owner.WriterAccesses++; throw new InvalidOperationException("Read-only test must not obtain writer"); } }
            public async ValueTask<ushort[]> ReadRegistersAsync(string bus, byte unit, ushort address, ushort count, CancellationToken ct = default)
            {
                owner.Reads++; await Task.Delay(1, ct);
                if (owner.FailRead) throw new IOException("test failure");
                return new ushort[count];
            }
            public ValueTask DisposeAsync() { if (!_disposed) { owner.Active--; _disposed = true; } return ValueTask.CompletedTask; }
        }
    }
    private sealed class Stream : IStreamResource
    {
        public bool Disposed;
        public int InfiniteTimeout => -1;
        public int ReadTimeout { get; set; }
        public int WriteTimeout { get; set; }
        public void DiscardInBuffer() { }
        public int Read(byte[] bytes, int offset, int count) { bytes[offset] = 0xAA; bytes[offset + 1] = 0xBB; return 2; }
        public void Write(byte[] bytes, int offset, int count) { Assert.Equal(new byte[] { 1, 3 }, bytes.AsSpan(offset, count).ToArray()); }
        public void Dispose() => Disposed = true;
    }
}
