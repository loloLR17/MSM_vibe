using TR2.Domain;
using Xunit;

namespace TR2.Application.Tests;

public sealed class CommandBusOrchestratorTests
{
    private static readonly DateTimeOffset Now =
        new(2026, 9, 9, 8, 0, 0, TimeSpan.Zero);

    [Fact]
    public async Task Prepared_transaction_stays_prepared_while_waiting_for_bus()
    {
        var scheduler = new BusWorkScheduler();
        var orchestrator = new CommandBusOrchestrator(scheduler);
        var coordinator = Coordinator(1001);
        var endpoint = Endpoint("RS485-A", 10);

        await orchestrator.PrepareAndQueueAsync(endpoint, coordinator, "START", Now);

        Assert.Equal(CommandTransactionState.Prepared, coordinator.ActiveTransaction?.State);
    }

    [Fact]
    public async Task Command_stays_prepared_when_bus_work_starts_before_protocol_submit()
    {
        var scheduler = new BusWorkScheduler();
        var orchestrator = new CommandBusOrchestrator(scheduler);
        var coordinator = Coordinator(1001);
        var endpoint = Endpoint("RS485-A", 10);
        var commandWork = await orchestrator.PrepareAndQueueAsync(
            endpoint,
            coordinator,
            "START",
            Now);

        var started = orchestrator.BeginNext(endpoint.Bus, Now);

        Assert.Equal(commandWork, started);
        Assert.Equal(CommandTransactionState.Prepared, coordinator.ActiveTransaction?.State);
    }

    [Fact]
    public async Task Command_work_keeps_priority_over_due_polling()
    {
        var scheduler = new BusWorkScheduler();
        var orchestrator = new CommandBusOrchestrator(scheduler);
        var coordinator = Coordinator(1001);
        var endpoint = Endpoint("RS485-A", 10);
        scheduler.QueuePolling(endpoint, PollingGroup.Fast, Now.AddMinutes(-1));
        var commandWork = await orchestrator.PrepareAndQueueAsync(
            endpoint,
            coordinator,
            "START",
            Now);

        Assert.Equal(commandWork, orchestrator.BeginNext(endpoint.Bus, Now));
    }

    [Fact]
    public async Task Ambiguous_transaction_blocks_new_command_and_can_queue_reconciliation()
    {
        var scheduler = new BusWorkScheduler();
        var orchestrator = new CommandBusOrchestrator(scheduler);
        var coordinator = Coordinator(1001);
        var endpoint = Endpoint("RS485-A", 10);
        var commandWork = await orchestrator.PrepareAndQueueAsync(
            endpoint,
            coordinator,
            "START",
            Now);

        orchestrator.BeginNext(endpoint.Bus, Now);
        coordinator.MarkSubmitted();
        orchestrator.Complete(endpoint.Bus, commandWork.WorkId);
        coordinator.MarkAmbiguous();

        await Assert.ThrowsAsync<InvalidOperationException>(async () =>
            await coordinator.PrepareAsync("STOP"));

        var reconciliation = orchestrator.QueueReconciliation(
            endpoint,
            coordinator,
            Now.AddSeconds(1));

        Assert.Equal(
            BusWorkKind.TransactionReconciliation,
            reconciliation.Kind);
        Assert.Equal(
            CommandTransactionState.Ambiguous,
            coordinator.ActiveTransaction?.State);
    }

    [Fact]
    public void Reconciliation_without_ambiguous_transaction_is_rejected()
    {
        var scheduler = new BusWorkScheduler();
        var orchestrator = new CommandBusOrchestrator(scheduler);
        var coordinator = Coordinator(1001);
        var endpoint = Endpoint("RS485-A", 10);

        Assert.Throws<InvalidOperationException>(() =>
            orchestrator.QueueReconciliation(endpoint, coordinator, Now));
    }

    [Fact]
    public async Task Terminal_resolution_after_reconciliation_allows_next_transaction()
    {
        var scheduler = new BusWorkScheduler();
        var orchestrator = new CommandBusOrchestrator(scheduler);
        var coordinator = Coordinator(1001);
        var endpoint = Endpoint("RS485-A", 10);
        var commandWork = await orchestrator.PrepareAndQueueAsync(
            endpoint,
            coordinator,
            "START",
            Now);

        orchestrator.BeginNext(endpoint.Bus, Now);
        coordinator.MarkSubmitted();
        orchestrator.Complete(endpoint.Bus, commandWork.WorkId);
        var ambiguous = coordinator.MarkAmbiguous();
        var reconciliation = orchestrator.QueueReconciliation(
            endpoint,
            coordinator,
            Now);

        Assert.Equal(reconciliation, orchestrator.BeginNext(endpoint.Bus, Now));
        coordinator.ResolveTerminal(ambiguous.TransactionId);
        orchestrator.Complete(endpoint.Bus, reconciliation.WorkId);

        var next = await coordinator.PrepareAsync("STOP");

        Assert.Equal((ushort)2, next.TransactionId.Value);
    }

    private static CommandCoordinator Coordinator(uint deviceId) =>
        new(new DeviceId(deviceId), new InMemoryReservationStore());

    private static TR2Endpoint Endpoint(string bus, byte address) =>
        new(new SerialBus(bus), new ModbusAddress(address));

    private sealed class InMemoryReservationStore : ICommandTransactionReservationStore
    {
        private readonly Dictionary<DeviceId, TransactionId> _last = [];

        public ValueTask<TransactionId?> GetLastAllocatedAsync(
            DeviceId deviceId,
            CancellationToken cancellationToken = default)
        {
            return ValueTask.FromResult(
                _last.TryGetValue(deviceId, out var value)
                    ? (TransactionId?)value
                    : null);
        }

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
