using TR2.Domain;

namespace TR2.Protocol;

public interface IB5CommandWriter
{
    ValueTask PrepareAsync(
        TR2Endpoint endpoint,
        B5CommandRequest request,
        CancellationToken cancellationToken = default);

    ValueTask SubmitAsync(
        TR2Endpoint endpoint,
        CancellationToken cancellationToken = default);
}
