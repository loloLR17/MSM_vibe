using TR2.Domain;
using Xunit;

namespace TR2.Application.Tests;

public sealed class FleetRefreshPlannerTests
{
    private static readonly DateTimeOffset DueAt =
        new(2026, 9, 9, 10, 0, 0, TimeSpan.Zero);

    [Fact]
    public void Compatible_session_queues_B1_through_B7_as_priority_refresh_work()
    {
        var scheduler = new BusWorkScheduler();
        var planner = new FleetRefreshPlanner(scheduler);
        var endpoint = Endpoint();
        var session = TR2Session.CreateCompatible(
            endpoint,
            new TR2Device(new DeviceId(1001)));

        var refreshes = planner.QueuePostReconnectRefresh(session, DueAt);

        Assert.Equal(7, refreshes.Count);
        Assert.Equal(TR2PollingPlan.PostReconnectRefreshBlocks, refreshes.Select(item => item.Block));
        Assert.All(refreshes, item => Assert.Equal(BusWorkKind.ExplicitRefresh, item.Work.Kind));
        Assert.All(refreshes, item => Assert.True(item.Work.IsPriority));
        Assert.All(refreshes, item => Assert.Equal(endpoint, item.Work.Endpoint));
    }

    [Fact]
    public void B0_is_not_queued_again_in_post_reconnect_refresh()
    {
        var planner = new FleetRefreshPlanner(new BusWorkScheduler());
        var session = TR2Session.CreateCompatible(
            Endpoint(),
            new TR2Device(new DeviceId(1001)));

        var refreshes = planner.QueuePostReconnectRefresh(session, DueAt);

        Assert.DoesNotContain(refreshes, item => item.Block == TR2RegisterBlock.B0);
    }

    [Fact]
    public void Post_reconnect_refresh_requires_compatible_identified_session()
    {
        var planner = new FleetRefreshPlanner(new BusWorkScheduler());
        var endpoint = Endpoint();

        Assert.Throws<InvalidOperationException>(() =>
            planner.QueuePostReconnectRefresh(TR2Session.CreateUnidentified(endpoint), DueAt));
        Assert.Throws<InvalidOperationException>(() =>
            planner.QueuePostReconnectRefresh(TR2Session.CreateIncompatible(endpoint), DueAt));
    }

    [Fact]
    public void Refresh_work_precedes_due_ordinary_polling()
    {
        var scheduler = new BusWorkScheduler();
        var planner = new FleetRefreshPlanner(scheduler);
        var endpoint = Endpoint();
        scheduler.QueuePolling(endpoint, PollingGroup.Fast, DueAt);
        var session = TR2Session.CreateCompatible(
            endpoint,
            new TR2Device(new DeviceId(1001)));
        var refreshes = planner.QueuePostReconnectRefresh(session, DueAt);

        var next = scheduler.BeginNext(endpoint.Bus, DueAt);

        Assert.NotNull(next);
        Assert.Equal(refreshes[0].Work.WorkId, next.WorkId);
        Assert.Equal(BusWorkKind.ExplicitRefresh, next.Kind);
    }

    private static TR2Endpoint Endpoint() =>
        new(new SerialBus("RS485-A"), new ModbusAddress(10));
}
