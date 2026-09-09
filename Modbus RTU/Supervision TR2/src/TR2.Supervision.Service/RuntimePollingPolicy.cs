using TR2.Application;

namespace TR2.Supervision.Service;

public sealed record RuntimePollingPolicy(
    TimeSpan StaticRetryInterval,
    TimeSpan FastInterval,
    TimeSpan MediumInterval,
    TimeSpan SlowInterval,
    TimeSpan ScanInterval)
{
    public static RuntimePollingPolicy Default { get; } = new(
        TimeSpan.FromSeconds(5),
        TimeSpan.FromSeconds(1),
        TimeSpan.FromSeconds(5),
        TimeSpan.FromSeconds(30),
        TimeSpan.FromMilliseconds(100));

    public TimeSpan GetInterval(PollingGroup group) => group switch
    {
        PollingGroup.Static => StaticRetryInterval,
        PollingGroup.Fast => FastInterval,
        PollingGroup.Medium => MediumInterval,
        PollingGroup.Slow => SlowInterval,
        _ => throw new ArgumentOutOfRangeException(nameof(group))
    };
}
