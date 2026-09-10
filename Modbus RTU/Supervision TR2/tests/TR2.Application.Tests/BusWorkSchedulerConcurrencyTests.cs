using System.Collections.Concurrent;
using TR2.Domain;
using Xunit;

namespace TR2.Application.Tests;

public sealed class BusWorkSchedulerConcurrencyTests
{
    [Fact]
    public async Task Concurrent_priority_publication_keeps_unique_ids_context_ready_and_one_active_work_per_bus()
    {
        const int workCount = 300;
        var scheduler = new BusWorkScheduler();
        var endpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(10));
        var dueAt = new DateTimeOffset(2026, 9, 10, 11, 0, 0, TimeSpan.Zero);
        var publishedContext = new ConcurrentDictionary<long, byte>();
        var consumed = new ConcurrentBag<long>();
        var producersDone = 0;

        var consumer = Task.Run(async () =>
        {
            while (Volatile.Read(ref producersDone) == 0 || consumed.Count < workCount)
            {
                var work = scheduler.BeginNext(endpoint.Bus, dueAt.AddSeconds(1));
                if (work is null)
                {
                    await Task.Yield();
                    continue;
                }

                Assert.True(publishedContext.ContainsKey(work.WorkId));
                Assert.Null(scheduler.BeginNext(endpoint.Bus, dueAt.AddSeconds(1)));
                consumed.Add(work.WorkId);
                scheduler.Complete(endpoint.Bus, work.WorkId);
            }
        });

        var producers = Enumerable.Range(0, workCount)
            .Select(_ => Task.Run(() => scheduler.QueuePriority(
                endpoint,
                BusWorkKind.ExplicitRefresh,
                dueAt,
                work => Assert.True(publishedContext.TryAdd(work.WorkId, 1)))))
            .ToArray();

        var queued = await Task.WhenAll(producers);
        Volatile.Write(ref producersDone, 1);
        await consumer;

        Assert.Equal(workCount, queued.Select(work => work.WorkId).Distinct().Count());
        Assert.Equal(workCount, consumed.Distinct().Count());
        Assert.Equal(
            queued.Select(work => work.WorkId).OrderBy(id => id),
            consumed.OrderBy(id => id));
        Assert.Null(scheduler.BeginNext(endpoint.Bus, dueAt.AddSeconds(1)));
    }
}
