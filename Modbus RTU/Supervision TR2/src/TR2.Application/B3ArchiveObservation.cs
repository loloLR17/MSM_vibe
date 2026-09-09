using TR2.Domain;
using TR2.Protocol;

namespace TR2.Application;

public sealed record B3ArchiveObservation(
    DeviceId DeviceId,
    B3VibrationSupervision Value,
    DateTimeOffset ReceivedAt);

public interface IB3ArchiveSink
{
    ValueTask AppendAsync(
        B3ArchiveObservation observation,
        CancellationToken cancellationToken = default);
}
