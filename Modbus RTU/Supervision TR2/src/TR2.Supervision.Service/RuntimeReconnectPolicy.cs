namespace TR2.Supervision.Service;

public sealed record RuntimeReconnectPolicy
{
    public RuntimeReconnectPolicy(TimeSpan interval)
    {
        if (interval <= TimeSpan.Zero)
        {
            throw new ArgumentOutOfRangeException(nameof(interval));
        }

        Interval = interval;
    }

    public TimeSpan Interval { get; }
}
