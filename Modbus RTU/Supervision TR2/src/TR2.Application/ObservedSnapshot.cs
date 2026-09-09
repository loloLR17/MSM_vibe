namespace TR2.Application;

public sealed record ObservedSnapshot<T>
{
    private ObservedSnapshot(bool hasValue, T? value, DateTimeOffset? receivedAt, bool isAvailable)
    {
        HasValue = hasValue;
        LastValue = value;
        ReceivedAt = receivedAt;
        IsAvailable = isAvailable;
    }

    public bool HasValue { get; }

    public T? LastValue { get; }

    public DateTimeOffset? ReceivedAt { get; }

    public bool IsAvailable { get; }

    public static ObservedSnapshot<T> NeverReceived() =>
        new(false, default, null, true);

    public ObservedSnapshot<T> Receive(T value, DateTimeOffset receivedAt) =>
        new(true, value, receivedAt, true);

    public ObservedSnapshot<T> MarkUnavailable() =>
        new(HasValue, LastValue, ReceivedAt, false);

    public SnapshotFreshness GetFreshness(
        SnapshotFreshnessPolicy policy,
        DateTimeOffset observedAt)
    {
        ArgumentNullException.ThrowIfNull(policy);

        if (!HasValue || ReceivedAt is null)
        {
            return SnapshotFreshness.NeverReceived;
        }

        if (!IsAvailable)
        {
            return SnapshotFreshness.Unavailable;
        }

        return policy.Evaluate(ReceivedAt.Value, observedAt);
    }
}
