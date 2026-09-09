namespace TR2.Supervision.Service;

public sealed record RuntimeReconnectPolicy(TimeSpan Interval)
{
    public RuntimeReconnectPolicy
    {
        if (Interval <= TimeSpan.Zero)
        {
            throw new ArgumentOutOfRangeException(nameof(Interval));
        }
    }
}
