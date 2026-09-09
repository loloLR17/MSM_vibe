using Xunit;

namespace TR2.Application.Tests;

public sealed class TR2PollingPlanTests
{
    [Theory]
    [InlineData(TR2RegisterBlock.B1, PollingGroup.Fast)]
    [InlineData(TR2RegisterBlock.B3, PollingGroup.Fast)]
    [InlineData(TR2RegisterBlock.B5, PollingGroup.Fast)]
    [InlineData(TR2RegisterBlock.B2, PollingGroup.Medium)]
    [InlineData(TR2RegisterBlock.B7, PollingGroup.Medium)]
    [InlineData(TR2RegisterBlock.B4, PollingGroup.Slow)]
    [InlineData(TR2RegisterBlock.B6, PollingGroup.Slow)]
    [InlineData(TR2RegisterBlock.B0, PollingGroup.Static)]
    public void Blocks_follow_the_frozen_S0_polling_groups(
        TR2RegisterBlock block,
        PollingGroup expected)
    {
        Assert.Equal(expected, TR2PollingPlan.GetGroup(block));
    }

    [Fact]
    public void Post_reconnect_refresh_contains_B1_through_B7_after_B0_identity_check()
    {
        Assert.Equal(
            [
                TR2RegisterBlock.B1,
                TR2RegisterBlock.B2,
                TR2RegisterBlock.B3,
                TR2RegisterBlock.B4,
                TR2RegisterBlock.B5,
                TR2RegisterBlock.B6,
                TR2RegisterBlock.B7
            ],
            TR2PollingPlan.PostReconnectRefreshBlocks);
    }

    [Fact]
    public void Polling_plan_does_not_embed_polling_intervals()
    {
        var publicProperties = typeof(TR2PollingPlan).GetProperties();

        Assert.DoesNotContain(publicProperties, property => property.PropertyType == typeof(TimeSpan));
    }
}
