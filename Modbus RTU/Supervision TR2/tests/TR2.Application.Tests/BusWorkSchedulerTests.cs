using TR2.Domain;
using Xunit;

namespace TR2.Application.Tests;

public sealed class BusWorkSchedulerTests
{
    private static readonly DateTimeOffset Now =
        new(2026, 9, 9, 8, 0, 0, TimeSpan.Zero);

    [Fact]
    public void Polling_groups_are_classification_only_and_do_not_embed_frequency()
    {
        var scheduler = new BusWorkScheduler();
        var endpoint = Endpoint("RS485-A", 10);

        var work = scheduler.QueuePolling(endpoint, PollingGroup.Fast, Now.AddMinutes(17));

        Assert.Equal(PollingGroup.Fast, work.PollingGroup);
        Assert.Equal(Now.AddMinutes(17), work.DueAt);
    }

    [Fact]
    public void Priority_work_precedes_due_ordinary_polling()
    {
        var scheduler = new BusWorkScheduler();
        var endpoint = Endpoint("RS485-A", 10);
        scheduler.QueuePolling(endpoint, PollingGroup.Fast, Now.AddMinutes(-1));
        var refresh = scheduler.QueuePriority(endpoint, BusWorkKind.ExplicitRefresh, Now);

        var next = scheduler.BeginNext(endpoint.Bus, Now);

        Assert.Equal(refresh, next);
    }

    [Fact]
    public void Priority_kinds_are_fifo_when_same_due_time()
    {
        var scheduler = new BusWorkScheduler();
        var endpoint = Endpoint("RS485-A", 10);
        var first = scheduler.QueuePriority(endpoint, BusWorkKind.CommandTransaction, Now);
        scheduler.QueuePriority(endpoint, BusWorkKind.TransactionReconciliation, Now);

        var next = scheduler.BeginNext(endpoint.Bus, Now);

        Assert.Equal(first, next);
    }

    [Fact]
    public void A_bus_has_at_most_one_active_work_item()
    {
        var scheduler = new BusWorkScheduler();
        var endpoint = Endpoint("RS485-A", 10);
        var first = scheduler.QueuePolling(endpoint, PollingGroup.Fast, Now);
        var second = scheduler.QueuePolling(endpoint, PollingGroup.Medium, Now);

        Assert.Equal(first, scheduler.BeginNext(endpoint.Bus, Now));
        Assert.Null(scheduler.BeginNext(endpoint.Bus, Now));

        scheduler.Complete(endpoint.Bus, first.WorkId);

        Assert.Equal(second, scheduler.BeginNext(endpoint.Bus, Now));
    }

    [Fact]
    public void Different_buses_can_have_work_active_independently()
    {
        var scheduler = new BusWorkScheduler();
        var endpointA = Endpoint("RS485-A", 10);
        var endpointB = Endpoint("RS485-B", 10);
        var workA = scheduler.QueuePolling(endpointA, PollingGroup.Fast, Now);
        var workB = scheduler.QueuePolling(endpointB, PollingGroup.Fast, Now);

        Assert.Equal(workA, scheduler.BeginNext(endpointA.Bus, Now));
        Assert.Equal(workB, scheduler.BeginNext(endpointB.Bus, Now));
    }

    [Fact]
    public void Future_work_is_not_started_early()
    {
        var scheduler = new BusWorkScheduler();
        var endpoint = Endpoint("RS485-A", 10);
        var due = Now.AddSeconds(30);
        var work = scheduler.QueuePolling(endpoint, PollingGroup.Slow, due);

        Assert.Null(scheduler.BeginNext(endpoint.Bus, due.AddTicks(-1)));
        Assert.Equal(work, scheduler.BeginNext(endpoint.Bus, due));
    }

    [Fact]
    public void Completing_unknown_work_is_rejected()
    {
        var scheduler = new BusWorkScheduler();
        var bus = new SerialBus("RS485-A");

        Assert.Throws<InvalidOperationException>(() => scheduler.Complete(bus, 999));
    }

    [Fact]
    public void Polling_cannot_be_queued_through_priority_path()
    {
        var scheduler = new BusWorkScheduler();
        var endpoint = Endpoint("RS485-A", 10);

        Assert.Throws<ArgumentOutOfRangeException>(() =>
            scheduler.QueuePriority(endpoint, BusWorkKind.Polling, Now));
    }

    [Fact]
    public void Sustained_mixed_work_preserves_one_active_item_per_bus_and_drains_all_work()
    {
        const int workPerBus = 400;
        var scheduler = new BusWorkScheduler();
        var endpointA = Endpoint("RS485-A", 10);
        var endpointB = Endpoint("RS485-B", 20);
        var expectedA = new HashSet<long>();
        var expectedB = new HashSet<long>();

        for (var index = 0; index < workPerBus; index++)
        {
            var dueAt = Now.AddMilliseconds(index % 7);
            var workA = index % 5 == 0
                ? scheduler.QueuePriority(endpointA, BusWorkKind.ExplicitRefresh, dueAt)
                : scheduler.QueuePolling(endpointA, PollingGroup.Fast, dueAt);
            var workB = index % 3 == 0
                ? scheduler.QueuePriority(endpointB, BusWorkKind.CommandTransaction, dueAt)
                : scheduler.QueuePolling(endpointB, PollingGroup.Medium, dueAt);

            Assert.True(expectedA.Add(workA.WorkId));
            Assert.True(expectedB.Add(workB.WorkId));
        }

        var completedA = new HashSet<long>();
        var completedB = new HashSet<long>();
        var observedAt = Now.AddSeconds(1);

        for (var index = 0; index < workPerBus; index++)
        {
            var activeA = Assert.IsType<ScheduledBusWork>(scheduler.BeginNext(endpointA.Bus, observedAt));
            Assert.Null(scheduler.BeginNext(endpointA.Bus, observedAt));
            Assert.True(completedA.Add(activeA.WorkId));

            var activeB = Assert.IsType<ScheduledBusWork>(scheduler.BeginNext(endpointB.Bus, observedAt));
            Assert.Null(scheduler.BeginNext(endpointB.Bus, observedAt));
            Assert.True(completedB.Add(activeB.WorkId));

            scheduler.Complete(endpointA.Bus, activeA.WorkId);
            scheduler.Complete(endpointB.Bus, activeB.WorkId);
        }

        Assert.Equal(expectedA, completedA);
        Assert.Equal(expectedB, completedB);
        Assert.Null(scheduler.BeginNext(endpointA.Bus, observedAt));
        Assert.Null(scheduler.BeginNext(endpointB.Bus, observedAt));
    }

    private static TR2Endpoint Endpoint(string bus, byte address) =>
        new(new SerialBus(bus), new ModbusAddress(address));
}
