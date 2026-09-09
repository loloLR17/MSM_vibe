using TR2.Domain;

namespace TR2.Protocol;

public interface IB0SessionReader
{
    ValueTask<TR2Session> ReadSessionAsync(
        TR2Endpoint endpoint,
        ushort supportedProtocolVersion,
        CancellationToken cancellationToken = default);
}
