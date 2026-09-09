using TR2.Domain;
using TR2.Protocol;
using Xunit;

namespace TR2.Application.Tests;

public sealed class RecoveredCommandReconciliationPlannerTests
{
    private static readonly DateTimeOffset Now =
        new(2026, 9, 9, 10, 0, 0, TimeSpan.Zero);

    [Fact]
    public async Task Reconnect_queues_reconciliation_before_post_reconnect_refreshes_for_recovered_device()
    {
        var scheduler = new BusWorkScheduler();
        var orchestrator = new CommandBusOrchestrator(scheduler);
        var coordinators = new CommandCoordinatorRegistry();
        var coordinator = new CommandCoordinator(new DeviceId(1001), new InMemoryReservationStore());
        await coordinator.PrepareAsync("START");
        coordinator.MarkSubmitted();
        coordinator.MarkAmbiguous();
        coordinators.Register(coordinator);

        var commandPlanner = new RecoveredCommandReconciliationPlanner(coordinators, orchestrator);
        var refreshPlanner = new FleetRefreshPlanner(scheduler);
        var fleet = new FleetRegistry();
        var endpoint = Endpoint("RS485-A", 10);
        fleet.RegisterEndpoint(endpoint);
        var discovery = new FleetDiscoveryService(
            fleet,
            new CompatibleReader(new DeviceId(1001)),
            1,
            refreshPlanner,
            commandPlanner);

        var result = await discovery.ReconnectAsync(endpoint, Now);
        var first = scheduler.BeginNext(endpoint.Bus, Now);

        Assert.Equal(TR2SessionState.Compatible, result.Session.State);
        Assert.Equal(7, result.Refreshes.Count);
        Assert.NotNull(first);
        Assert.Equal(BusWorkKind.TransactionReconciliation, first!.Kind);
        Assert.Equal(endpoint, first.Endpoint);
    }

    [Fact]
    public async Task Reconnect_does_not_queue_reconciliation_for_another_device_id()
    {
        var scheduler = new BusWorkScheduler();
        var orchestrator = new CommandBusOrchestrator(scheduler);
        var coordinators = new CommandCoordinatorRegistry();
        var coordinator = new CommandCoordinator(new DeviceId(1001), new InMemoryReservationStore());
        await coordinator.PrepareAsync("START");
        coordinator.MarkSubmitted();
        coordinator.MarkAmbiguous();
        coordinators.Register(coordinator);

        var fleet = new FleetRegistry();
        var endpoint = Endpoint("RS485-A", 10);
        fleet.RegisterEndpoint(endpoint);
        var discovery = new FleetDiscoveryService(
            fleet,
            new CompatibleReader(new DeviceId(2002)),
            1,
            new FleetRefreshPlanner(scheduler),
            new RecoveredCommandReconciliationPlanner(coordinators, orchestrator));

        await discovery.ReconnectAsync(endpoint, Now);
        var first = scheduler.BeginNext(endpoint.Bus, Now);

        Assert.NotNull(first);
        Assert.Equal(BusWorkKind.ExplicitRefresh, first!.Kind);
    }

    [Fact]
    public void Registry_rejects_multiple_coordinators_for_same_device_id()
    {
        var registry = new CommandCoordinatorRegistry();
        registry.Register(new CommandCoordinator(new DeviceId(1001), new InMemoryReservationStore()));

        Assert.Throws<InvalidOperationException>(() =>
            registry.Register(new CommandCoordinator(new DeviceId(1001), new InMemoryReservationStore())));
    }

    private static TR2Endpoint Endpoint(string bus, byte address) =>
        new(new SerialBus(bus), new ModbusAddress(address));

    private sealed class CompatibleReader : IB0SessionReader
    {
        private readonly DeviceId _deviceId;

        public CompatibleReader(DeviceId deviceId) => _deviceId = deviceId;

        public ValueTask<TR2Session> ReadSessionAsync(
            TR2Endpoint endpoint,
            ushort supportedProtocolVersion,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromResult(TR2Session.CreateCompatible(endpoint, new TR2Device(_deviceId)));
    }

    private sealed class InMemoryReservationStore : ICommandTransactionReservationStore
    {
        private TransactionId? _last;

        public ValueTask<TransactionId?> GetLastAllocatedAsync(
            DeviceId deviceId,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromResult(_last);

        public ValueTask PersistAllocationAsync(
            DeviceId deviceId,
            TransactionId transactionId,
            CancellationToken cancellationToken = default)
        {
            _last = transactionId;
            return ValueTask.CompletedTask;
        }
    }
}
