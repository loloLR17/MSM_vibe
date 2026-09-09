using TR2.Domain;
using TR2.Protocol;
using Xunit;

namespace TR2.Application.Tests;

public sealed class CommandAutomaticMonitoringChainTests
{
    private static readonly DateTimeOffset T0 =
        new(2026, 9, 9, 13, 0, 0, TimeSpan.Zero);

    [Fact]
    public async Task Successful_command_execution_queues_post_submit_monitoring()
    {
        var scheduler = new BusWorkScheduler();
        var writer = new StubWriter();
        var orchestrator = new CommandBusOrchestrator(
            scheduler,
            null,
            new B5CommandExecutionService(writer),
            null);
        var coordinator = Coordinator();
        var endpoint = Endpoint();
        var commandWork = await orchestrator.PrepareAndQueueAsync(
            endpoint,
            coordinator,
            "START",
            T0);
        var activeCommand = orchestrator.BeginNext(endpoint.Bus, T0)!;

        var monitoring = await orchestrator.ExecuteCommandAndQueueMonitoringAsync(
            activeCommand,
            new B5CommandRequest(3, 1, 0, 0, 0, 0),
            T0.AddSeconds(1),
            T0.AddSeconds(2),
            T0.AddSeconds(10));

        Assert.Equal(CommandTransactionState.Submitted, coordinator.ActiveTransaction?.State);
        Assert.Equal(BusWorkKind.CommandPostSubmitMonitoring, monitoring.Kind);
        Assert.Equal(T0.AddSeconds(2), monitoring.DueAt);
        Assert.Null(orchestrator.BeginNext(endpoint.Bus, T0.AddSeconds(1)));
        Assert.Equal(monitoring, orchestrator.BeginNext(endpoint.Bus, T0.AddSeconds(2)));
    }

    [Fact]
    public async Task Preparation_failure_queues_no_monitoring_and_leaves_transaction_prepared()
    {
        var scheduler = new BusWorkScheduler();
        var writer = new StubWriter { FailPrepare = true };
        var orchestrator = new CommandBusOrchestrator(
            scheduler,
            null,
            new B5CommandExecutionService(writer),
            null);
        var coordinator = Coordinator();
        var endpoint = Endpoint();
        await orchestrator.PrepareAndQueueAsync(endpoint, coordinator, "START", T0);
        var activeCommand = orchestrator.BeginNext(endpoint.Bus, T0)!;

        await Assert.ThrowsAsync<IOException>(async () =>
            await orchestrator.ExecuteCommandAndQueueMonitoringAsync(
                activeCommand,
                new B5CommandRequest(3, 1, 0, 0, 0, 0),
                T0.AddSeconds(1),
                T0.AddSeconds(2),
                T0.AddSeconds(10)));

        Assert.Equal(CommandTransactionState.Prepared, coordinator.ActiveTransaction?.State);
        var polling = scheduler.QueuePolling(endpoint, PollingGroup.Fast, T0);
        Assert.Equal(polling, scheduler.BeginNext(endpoint.Bus, T0.AddSeconds(2)));
    }

    [Fact]
    public async Task Submit_failure_queues_no_normal_monitoring_and_leaves_transaction_ambiguous()
    {
        var scheduler = new BusWorkScheduler();
        var writer = new StubWriter { FailSubmit = true };
        var orchestrator = new CommandBusOrchestrator(
            scheduler,
            null,
            new B5CommandExecutionService(writer),
            null);
        var coordinator = Coordinator();
        var endpoint = Endpoint();
        await orchestrator.PrepareAndQueueAsync(endpoint, coordinator, "START", T0);
        var activeCommand = orchestrator.BeginNext(endpoint.Bus, T0)!;

        await Assert.ThrowsAsync<IOException>(async () =>
            await orchestrator.ExecuteCommandAndQueueMonitoringAsync(
                activeCommand,
                new B5CommandRequest(3, 1, 0, 0, 0, 0),
                T0.AddSeconds(1),
                T0.AddSeconds(2),
                T0.AddSeconds(10)));

        Assert.Equal(CommandTransactionState.Ambiguous, coordinator.ActiveTransaction?.State);
        var polling = scheduler.QueuePolling(endpoint, PollingGroup.Fast, T0);
        Assert.Equal(polling, scheduler.BeginNext(endpoint.Bus, T0.AddSeconds(2)));
    }

    private static CommandCoordinator Coordinator() =>
        new(new DeviceId(1001), new InMemoryReservationStore());

    private static TR2Endpoint Endpoint() =>
        new(new SerialBus("RS485-A"), new ModbusAddress(10));

    private sealed class StubWriter : IB5CommandWriter
    {
        public bool FailPrepare { get; init; }
        public bool FailSubmit { get; init; }

        public ValueTask PrepareAsync(
            TR2Endpoint endpoint,
            B5CommandRequest request,
            CancellationToken cancellationToken = default)
        {
            if (FailPrepare)
            {
                throw new IOException("Injected preparation failure.");
            }

            return ValueTask.CompletedTask;
        }

        public ValueTask SubmitAsync(
            TR2Endpoint endpoint,
            CancellationToken cancellationToken = default)
        {
            if (FailSubmit)
            {
                throw new IOException("Injected submit failure.");
            }

            return ValueTask.CompletedTask;
        }
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
