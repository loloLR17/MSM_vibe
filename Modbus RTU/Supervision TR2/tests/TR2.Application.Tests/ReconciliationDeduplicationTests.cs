using TR2.Domain;
using Xunit;

namespace TR2.Application.Tests;

public sealed class ReconciliationDeduplicationTests
{
    private static readonly DateTimeOffset T0 =
        new(2026, 9, 9, 12, 0, 0, TimeSpan.Zero);

    [Fact]
    public async Task Repeated_queue_for_same_ambiguous_transaction_returns_same_pending_work()
    {
        var bus = new SerialBus("RS485-A");
        var endpoint = new TR2Endpoint(bus, new ModbusAddress(10));
        var coordinator = await CreateAmbiguousCoordinatorAsync(1001);
        var orchestrator = new CommandBusOrchestrator(new BusWorkScheduler());

        var first = orchestrator.QueueReconciliation(
            endpoint,
            coordinator,
            T0.AddSeconds(1));
        var second = orchestrator.QueueReconciliation(
            endpoint,
            coordinator,
            T0.AddSeconds(2));

        Assert.Equal(first, second);
        Assert.Equal(first.WorkId, second.WorkId);
    }

    [Fact]
    public async Task Repeated_queue_while_reconciliation_is_active_returns_same_work()
    {
        var bus = new SerialBus("RS485-A");
        var endpoint = new TR2Endpoint(bus, new ModbusAddress(10));
        var coordinator = await CreateAmbiguousCoordinatorAsync(1001);
        var orchestrator = new CommandBusOrchestrator(new BusWorkScheduler());

        var queued = orchestrator.QueueReconciliation(endpoint, coordinator, T0);
        var active = orchestrator.BeginNext(bus, T0);

        Assert.Equal(queued, active);

        var duplicate = orchestrator.QueueReconciliation(
            endpoint,
            coordinator,
            T0.AddSeconds(1));

        Assert.Equal(active, duplicate);
    }

    [Fact]
    public async Task Completing_reconciliation_releases_deduplication_key()
    {
        var bus = new SerialBus("RS485-A");
        var endpoint = new TR2Endpoint(bus, new ModbusAddress(10));
        var coordinator = await CreateAmbiguousCoordinatorAsync(1001);
        var orchestrator = new CommandBusOrchestrator(new BusWorkScheduler());

        var first = orchestrator.QueueReconciliation(endpoint, coordinator, T0);
        Assert.Equal(first, orchestrator.BeginNext(bus, T0));
        orchestrator.Complete(bus, first.WorkId);

        var second = orchestrator.QueueReconciliation(
            endpoint,
            coordinator,
            T0.AddSeconds(1));

        Assert.NotEqual(first.WorkId, second.WorkId);
    }

    [Fact]
    public async Task Different_devices_keep_independent_reconciliation_work()
    {
        var bus = new SerialBus("RS485-A");
        var endpoint1 = new TR2Endpoint(bus, new ModbusAddress(10));
        var endpoint2 = new TR2Endpoint(bus, new ModbusAddress(11));
        var coordinator1 = await CreateAmbiguousCoordinatorAsync(1001);
        var coordinator2 = await CreateAmbiguousCoordinatorAsync(1002);
        var orchestrator = new CommandBusOrchestrator(new BusWorkScheduler());

        var first = orchestrator.QueueReconciliation(endpoint1, coordinator1, T0);
        var second = orchestrator.QueueReconciliation(endpoint2, coordinator2, T0);

        Assert.NotEqual(first.WorkId, second.WorkId);
    }

    private static async Task<CommandCoordinator> CreateAmbiguousCoordinatorAsync(ulong deviceId)
    {
        var coordinator = new CommandCoordinator(
            new DeviceId(deviceId),
            new InMemoryReservationStore());

        await coordinator.PrepareAsync("START");
        coordinator.MarkAmbiguousAfterSubmitAttempt();
        return coordinator;
    }

    private sealed class InMemoryReservationStore : ICommandTransactionReservationStore
    {
        private readonly Dictionary<DeviceId, TransactionId> _last = [];

        public ValueTask<TransactionId?> GetLastAllocatedAsync(
            DeviceId deviceId,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromResult(
                _last.TryGetValue(deviceId, out var value)
                    ? (TransactionId?)value
                    : null);

        public ValueTask PersistAllocationAsync(
            DeviceId deviceId,
            TransactionId transactionId,
            CancellationToken cancellationToken = default)
        {
            _last[deviceId] = transactionId;
            return ValueTask.CompletedTask;
        }
    }
}
