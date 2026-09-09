namespace TR2.Application;

public sealed record SnapshotFreshnessPolicy
{
    public SnapshotFreshnessPolicy(TimeSpan agingAfter, TimeSpan staleAfter)
    {
        if (agingAfter < TimeSpan.Zero)
        {
            throw new ArgumentOutOfRangeException(nameof(agingAfter));
        }

        if (staleAfter <= agingAfter)
        {
            throw new ArgumentOutOfRangeException(
                nameof(staleAfter),
                "staleAfter must be greater than agingAfter.");
        }

        AgingAfter = agingAfter;
        StaleAfter = staleAfter;
    }

    public TimeSpan AgingAfter { get; }

    public TimeSpan StaleAfter { get; }

    public SnapshotFreshness Evaluate(DateTimeOffset receivedAt, DateTimeOffset observedAt)
    {
        if (observedAt < receivedAt)
        {
            throw new ArgumentOutOfRangeException(
                nameof(observedAt),
                "observedAt must not be earlier than receivedAt.");
        }

        var age = observedAt - receivedAt;

        if (age <= AgingAfter)
        {
            return SnapshotFreshness.Fresh;
        }

        return age < StaleAfter
            ? SnapshotFreshness.Aging
            : SnapshotFreshness.Stale;
    }
}
