using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Application.Tests;

public sealed class ExplicitRefreshExecutorTests
{
    private static readonly DateTimeOffset Now =
        new(2026, 9, 9, 14, 0, 0, TimeSpan.Zero);

    [Theory]
    [InlineData(TR2RegisterBlock.B1, 1000)]
    [InlineData(TR2RegisterBlock.B2, 2000)]
    [InlineData(TR2RegisterBlock.B3, 3000)]
    [InlineData(TR2RegisterBlock.B4, 4000)]
    [InlineData(TR2RegisterBlock.B5, 5000)]
    [InlineData(TR2RegisterBlock.B6, 6000)]
    [InlineData(TR2RegisterBlock.B7, 7000)]
    public async Task Explicit_refresh_reads_exact_requested_block(
        TR2RegisterBlock block,
        int expectedStartAddress)
    {
        var transport = new RecordingTransport();
        var scheduler = new BusWorkScheduler();
        var executor = new ExplicitRefreshExecutor(scheduler, transport);
        var endpoint = Endpoint();
        var queued = scheduler.QueuePriority(endpoint, BusWorkKind.ExplicitRefresh, Now);
        var active = scheduler.BeginNext(endpoint.Bus, Now)!;

        var result = await executor.ExecuteAsync(new ScheduledBlockRefresh(active, block));

        Assert.Equal(queued, active);
        Assert.Equal(new[] { expectedStartAddress }, transport.StartAddresses.Select(value => (int)value));
        Assert.Equal(TR2PollingPlan.GetGroup(block), result.Group);
        AssertRequestedBlockPresent(result, block);
        Assert.Null(scheduler.BeginNext(endpoint.Bus, Now));
    }

    [Fact]
    public async Task Explicit_refresh_failure_releases_bus_and_propagates()
    {
        var transport = new RecordingTransport { FailAtStartAddress = 3000 };
        var scheduler = new BusWorkScheduler();
        var executor = new ExplicitRefreshExecutor(scheduler, transport);
        var endpoint = Endpoint();
        var queued = scheduler.QueuePriority(endpoint, BusWorkKind.ExplicitRefresh, Now);
        var active = scheduler.BeginNext(endpoint.Bus, Now)!;

        var error = await Assert.ThrowsAsync<IOException>(async () =>
            await executor.ExecuteAsync(
                new ScheduledBlockRefresh(active, TR2RegisterBlock.B3)));

        Assert.Equal("Injected explicit refresh failure.", error.Message);
        Assert.Equal(queued, active);

        var next = scheduler.QueuePolling(endpoint, PollingGroup.Fast, Now);
        Assert.Equal(next, scheduler.BeginNext(endpoint.Bus, Now));
    }

    [Fact]
    public async Task B0_is_not_repeated_by_post_reconnect_explicit_refresh_executor()
    {
        var scheduler = new BusWorkScheduler();
        var executor = new ExplicitRefreshExecutor(scheduler, new RecordingTransport());
        var endpoint = Endpoint();
        scheduler.QueuePriority(endpoint, BusWorkKind.ExplicitRefresh, Now);
        var active = scheduler.BeginNext(endpoint.Bus, Now)!;

        await Assert.ThrowsAsync<InvalidOperationException>(async () =>
            await executor.ExecuteAsync(
                new ScheduledBlockRefresh(active, TR2RegisterBlock.B0)));

        var next = scheduler.QueuePolling(endpoint, PollingGroup.Static, Now);
        Assert.Equal(next, scheduler.BeginNext(endpoint.Bus, Now));
    }

    private static void AssertRequestedBlockPresent(
        PollingReadSet result,
        TR2RegisterBlock block)
    {
        Assert.Equal(block == TR2RegisterBlock.B1, result.B1 is not null);
        Assert.Equal(block == TR2RegisterBlock.B2, result.B2 is not null);
        Assert.Equal(block == TR2RegisterBlock.B3, result.B3 is not null);
        Assert.Equal(block == TR2RegisterBlock.B4, result.B4 is not null);
        Assert.Equal(block == TR2RegisterBlock.B5, result.B5 is not null);
        Assert.Equal(block == TR2RegisterBlock.B6, result.B6 is not null);
        Assert.Equal(block == TR2RegisterBlock.B7, result.B7 is not null);
        Assert.Null(result.B0Session);
    }

    private static TR2Endpoint Endpoint() =>
        new(new SerialBus("RS485-A"), new ModbusAddress(10));

    private sealed class RecordingTransport : IRegisterTransport
    {
        public List<ushort> StartAddresses { get; } = [];
        public ushort? FailAtStartAddress { get; init; }

        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default)
        {
            StartAddresses.Add(startAddress);

            if (FailAtStartAddress == startAddress)
            {
                return ValueTask.FromException<ushort[]>(
                    new IOException("Injected explicit refresh failure."));
            }

            return ValueTask.FromResult(new ushort[registerCount]);
        }
    }
}
