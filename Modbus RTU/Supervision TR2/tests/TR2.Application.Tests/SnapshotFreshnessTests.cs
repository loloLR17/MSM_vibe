using Xunit;

namespace TR2.Application.Tests;

public sealed class SnapshotFreshnessTests
{
    private static readonly SnapshotFreshnessPolicy Policy =
        new(TimeSpan.FromSeconds(2), TimeSpan.FromSeconds(5));

    [Fact]
    public void Snapshot_is_never_received_before_first_value()
    {
        var snapshot = ObservedSnapshot<int>.NeverReceived();

        Assert.False(snapshot.HasValue);
        Assert.Null(snapshot.ReceivedAt);
        Assert.Equal(
            SnapshotFreshness.NeverReceived,
            snapshot.GetFreshness(Policy, DateTimeOffset.UtcNow));
    }

    [Theory]
    [InlineData(0, SnapshotFreshness.Fresh)]
    [InlineData(2, SnapshotFreshness.Fresh)]
    [InlineData(3, SnapshotFreshness.Aging)]
    [InlineData(5, SnapshotFreshness.Stale)]
    [InlineData(20, SnapshotFreshness.Stale)]
    public void Freshness_is_evaluated_from_explicit_policy(
        int ageSeconds,
        SnapshotFreshness expected)
    {
        var receivedAt = new DateTimeOffset(2026, 9, 9, 8, 0, 0, TimeSpan.Zero);
        var snapshot = ObservedSnapshot<int>.NeverReceived().Receive(42, receivedAt);

        Assert.Equal(
            expected,
            snapshot.GetFreshness(Policy, receivedAt.AddSeconds(ageSeconds)));
    }

    [Fact]
    public void Communication_loss_preserves_last_value_and_marks_it_unavailable()
    {
        var receivedAt = new DateTimeOffset(2026, 9, 9, 8, 0, 0, TimeSpan.Zero);
        var snapshot = ObservedSnapshot<int>.NeverReceived().Receive(42, receivedAt);

        var unavailable = snapshot.MarkUnavailable();

        Assert.True(unavailable.HasValue);
        Assert.Equal(42, unavailable.LastValue);
        Assert.Equal(receivedAt, unavailable.ReceivedAt);
        Assert.Equal(
            SnapshotFreshness.Unavailable,
            unavailable.GetFreshness(Policy, receivedAt.AddHours(1)));
    }

    [Fact]
    public void A_new_value_restores_availability_without_zeroing_history()
    {
        var firstAt = new DateTimeOffset(2026, 9, 9, 8, 0, 0, TimeSpan.Zero);
        var secondAt = firstAt.AddMinutes(1);
        var unavailable = ObservedSnapshot<int>
            .NeverReceived()
            .Receive(42, firstAt)
            .MarkUnavailable();

        var refreshed = unavailable.Receive(84, secondAt);

        Assert.True(refreshed.IsAvailable);
        Assert.Equal(84, refreshed.LastValue);
        Assert.Equal(secondAt, refreshed.ReceivedAt);
        Assert.Equal(SnapshotFreshness.Fresh, refreshed.GetFreshness(Policy, secondAt));
    }

    [Fact]
    public void Policy_rejects_invalid_threshold_order()
    {
        Assert.Throws<ArgumentOutOfRangeException>(() =>
            new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(-1), TimeSpan.FromSeconds(5)));

        Assert.Throws<ArgumentOutOfRangeException>(() =>
            new SnapshotFreshnessPolicy(TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(5)));
    }

    [Fact]
    public void Evaluation_rejects_time_before_reception()
    {
        var receivedAt = new DateTimeOffset(2026, 9, 9, 8, 0, 0, TimeSpan.Zero);
        var snapshot = ObservedSnapshot<int>.NeverReceived().Receive(42, receivedAt);

        Assert.Throws<ArgumentOutOfRangeException>(() =>
            snapshot.GetFreshness(Policy, receivedAt.AddTicks(-1)));
    }
}
