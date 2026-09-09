using TR2.Protocol;
using TR2.Transport;

namespace TR2.Application;

public sealed class ExplicitRefreshExecutor
{
    private readonly BusWorkScheduler _scheduler;
    private readonly B1Reader _b1;
    private readonly B2Reader _b2;
    private readonly B3Reader _b3;
    private readonly B4Reader _b4;
    private readonly B5Reader _b5;
    private readonly B6Reader _b6;
    private readonly B7Reader _b7;

    public ExplicitRefreshExecutor(
        BusWorkScheduler scheduler,
        IRegisterTransport transport)
    {
        ArgumentNullException.ThrowIfNull(scheduler);
        ArgumentNullException.ThrowIfNull(transport);

        _scheduler = scheduler;
        _b1 = new B1Reader(transport);
        _b2 = new B2Reader(transport);
        _b3 = new B3Reader(transport);
        _b4 = new B4Reader(transport);
        _b5 = new B5Reader(transport);
        _b6 = new B6Reader(transport);
        _b7 = new B7Reader(transport);
    }

    public async ValueTask<PollingReadSet> ExecuteAsync(
        ScheduledBlockRefresh refresh,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(refresh);

        if (refresh.Work.Kind != BusWorkKind.ExplicitRefresh)
        {
            throw new InvalidOperationException("The work item is not an explicit refresh work item.");
        }

        try
        {
            var endpoint = refresh.Work.Endpoint;
            var group = TR2PollingPlan.GetGroup(refresh.Block);

            return refresh.Block switch
            {
                TR2RegisterBlock.B1 => new PollingReadSet(
                    group, null,
                    await _b1.ReadAsync(endpoint, cancellationToken),
                    null, null, null, null, null, null),

                TR2RegisterBlock.B2 => new PollingReadSet(
                    group, null, null,
                    await _b2.ReadAsync(endpoint, cancellationToken),
                    null, null, null, null, null),

                TR2RegisterBlock.B3 => new PollingReadSet(
                    group, null, null, null,
                    await _b3.ReadAsync(endpoint, cancellationToken),
                    null, null, null, null),

                TR2RegisterBlock.B4 => new PollingReadSet(
                    group, null, null, null, null,
                    await _b4.ReadAsync(endpoint, cancellationToken),
                    null, null, null),

                TR2RegisterBlock.B5 => new PollingReadSet(
                    group, null, null, null, null, null,
                    await _b5.ReadAsync(endpoint, cancellationToken),
                    null, null),

                TR2RegisterBlock.B6 => new PollingReadSet(
                    group, null, null, null, null, null, null,
                    await _b6.ReadAsync(endpoint, cancellationToken),
                    null),

                TR2RegisterBlock.B7 => new PollingReadSet(
                    group, null, null, null, null, null, null, null,
                    await _b7.ReadAsync(endpoint, cancellationToken)),

                TR2RegisterBlock.B0 => throw new InvalidOperationException(
                    "B0 is re-read by reconnect discovery before post-reconnect explicit refreshes."),

                _ => throw new ArgumentOutOfRangeException(nameof(refresh.Block))
            };
        }
        finally
        {
            _scheduler.Complete(refresh.Work.Endpoint.Bus, refresh.Work.WorkId);
        }
    }
}
