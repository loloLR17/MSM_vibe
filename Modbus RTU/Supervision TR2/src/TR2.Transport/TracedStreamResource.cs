using NModbus.IO;

namespace TR2.Transport;

public sealed record SerialStreamTrace(DateTimeOffset ReceivedAt, string Direction, string Hex);

// Transparent byte trace below NModbus: records actual chunks, including incomplete/error replies.
// The observer must be local and bounded. No additional serial owner, retry or frame synthesis.
public sealed class TracedStreamResource(IStreamResource inner, Action<SerialStreamTrace> trace) : IStreamResource
{
    public int InfiniteTimeout => inner.InfiniteTimeout;
    public int ReadTimeout { get => inner.ReadTimeout; set => inner.ReadTimeout = value; }
    public int WriteTimeout { get => inner.WriteTimeout; set => inner.WriteTimeout = value; }
    public void DiscardInBuffer() => inner.DiscardInBuffer();
    public int Read(byte[] buffer, int offset, int count)
    {
        var n = inner.Read(buffer, offset, count);
        trace(new(DateTimeOffset.UtcNow, "rx", Convert.ToHexString(buffer.AsSpan(offset, n))));
        return n;
    }
    public void Write(byte[] buffer, int offset, int count)
    {
        trace(new(DateTimeOffset.UtcNow, "tx", Convert.ToHexString(buffer.AsSpan(offset, count))));
        inner.Write(buffer, offset, count);
    }
    public void Dispose() => inner.Dispose();
}
