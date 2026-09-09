using TR2.Domain;

namespace TR2.Protocol;

public interface IB5CommandStateReader
{
    ValueTask<B5CommandState> ReadAsync(
        TR2Endpoint endpoint,
        CancellationToken cancellationToken = default);
}
