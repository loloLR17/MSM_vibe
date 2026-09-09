using TR2.Domain;
using TR2.Protocol;
using Xunit;

namespace TR2.Application.Tests;

public sealed class CommandPostSubmitNextWorkTests
{
    private static readonly DateTimeOffset T0 =
        new(2026, 9, 9, 13, 0, 0, TimeSpan.Zero);

    [Fact]
    public async Task Pending_observation_schedules_next_monitoring_work()
    {
        var endpoint = Endpoint();
        var coordinator = await SubmittedCoordinatorAsync();
        var scheduler = new BusWorkScheduler();
        var orchestrator = new CommandBusOrchestrator(
            scheduler,
            null,
            null,
            new B5PostSubmitMonitor(
                new StubReader(State(activeTransactionId: 1, status: 3))));

        var first = orchestrator.QueuePostSubmitMonitoring(
            endpoint,
            coordinator,
            T0,
            T0.AddSeconds(10));
        var active = orchestrator.BeginNext(endpoint.Bus, T0)!;

        var cycle = await orchestrator.ExecutePostSubmitMonitoringAndScheduleNextAsync(
            active,
            T0.AddSeconds(1),
            T0.AddSeconds(2),
            T0.AddSeconds(3));

        Assert.Equal(B5PostSubmitOutcome.Pending, cycle.Observation.Outcome);
        Assert.NotNull(cycle.NextWork);
        Assert.Equal(BusWorkKind.CommandPostSubmitMonitoring, cycle.NextWork!.Kind);
        Assert.Equal(T0.AddSeconds(2), cycle.NextWork.DueAt);
        Assert.Equal(CommandTransactionState.Submitted, coordinator.ActiveTransaction?.State);
    }

    [Fact]
    public async Task Timeout_observation_schedules_reconciliation_work()
    {
        var endpoint = Endpoint();
        var coordinator = await SubmittedCoordinatorAsync();
        var scheduler = new BusWorkScheduler();
        var orchestrator = new CommandBusOrchestrator(
            scheduler,
            null,
            null,
            new B5PostSubmitMonitor(
                new StubReader(State(activeTransactionId: 1, status: 3))));

        orchestrator.QueuePostSubmitMonitoring(
            endpoint,
            coordinator,
            T0,
            T0.AddSeconds(5));
        var active = orchestrator.BeginNext(endpoint.Bus, T0)!;

        var cycle = await orchestrator.ExecutePostSubmitMonitoringAndScheduleNextAsync(
            active,
            T0.AddSeconds(5),
            T0.AddSeconds(6),
            T0.AddSeconds(7));

        Assert.Equal(B5PostSubmitOutcome.TimedOutAmbiguous, cycle.Observation.Outcome);
        Assert.NotNull(cycle.NextWork);
        Assert.Equal(BusWorkKind.TransactionReconciliation, cycle.NextWork!.Kind);
        Assert.Equal(T0.AddSeconds(7), cycle.NextWork.DueAt);
        Assert.Equal(CommandTransactionState.Ambiguous, coordinator.ActiveTransaction?.State);
    }

    [Fact]
    public async Task Terminal_observation_schedules_no_follow_up_work()
    {
        var endpoint = Endpoint();
        var coordinator = await SubmittedCoordinatorAsync();
        var scheduler = new BusWorkScheduler();
        var orchestrator = new CommandBusOrchestrator(
            scheduler,
            null,
            null,
            new B5PostSubmitMonitor(
                new StubReader(State(lastTransactionId: 1, lastStatusFinal: 4))));

        orchestrator.QueuePostSubmitMonitoring(
            endpoint,
            coordinator,
            T0,
            T0.AddSeconds(10));
        var active = orchestrator.BeginNext(endpoint.Bus, T0)!;

        var cycle = await orchestrator.ExecutePostSubmitMonitoringAndScheduleNextAsync(
            active,
            T0.AddSeconds(1),
            T0.AddSeconds(2),
            T0.AddSeconds(3));

        Assert.Equal(B5PostSubmitOutcome.TerminalEvidence, cycle.Observation.Outcome);
        Assert.Null(cycle.NextWork);
        Assert.Null(coordinator.ActiveTransaction);
        Assert.Null(scheduler.BeginNext(endpoint.Bus, T0.AddSeconds(4)));
    }

    [Fact]
    public async Task Read_failure_at_timeout_does_not_schedule_implicit_reconciliation()
    {
        var endpoint = Endpoint();
        var coordinator = await SubmittedCoordinatorAsync();
        var scheduler = new BusWorkScheduler();
        var orchestrator = new CommandBusOrchestrator(
            scheduler,
            null,
            null,
            new B5PostSubmitMonitor(new StubReader(new IOException("read failed"))));

        orchestrator.QueuePostSubmitMonitoring(
            endpoint,
            coordinator,
            T0,
            T0.AddSeconds(5));
        var active = orchestrator.BeginNext(endpoint.Bus, T0)!;

        await Assert.ThrowsAsync<IOException>(async () =>
            await orchestrator.ExecutePostSubmitMonitoringAndScheduleNextAsync(
                active,
                T0.AddSeconds(5),
                T0.AddSeconds(6),
                T0.AddSeconds(7)));

        Assert.Equal(CommandTransactionState.Ambiguous, coordinator.ActiveTransaction?.State);

        var polling = scheduler.QueuePolling(endpoint, PollingGroup.Fast, T0);
        Assert.Equal(polling, scheduler.BeginNext(endpoint.Bus, T0.AddSeconds(8)));
    }

    private static TR2Endpoint Endpoint() =>
        new(new SerialBus("RS485-A"), new ModbusAddress(10));

    private static async Task<CommandCoordinator> SubmittedCoordinatorAsync()
    {
        var coordinator = new CommandCoordinator(
            new DeviceId(1001),
            new ReservationStore());
        await coordinator.PrepareAsync("START");
        coordinator.MarkSubmitted();
        return coordinator;
    }

    private static B5CommandState State(
        ushort activeTransactionId = 0,
        ushort status = 0,
        ushort lastTransactionId = 0,
        ushort lastStatusFinal = 0) =>
        new(
            0, 0, 0, 0, 0, 0, 0,
            0, activeTransactionId, status, 0, 0, 0,
            0, lastTransactionId, lastStatusFinal, 0, 0);

    private sealed class StubReader : IB5CommandStateReader
    {
        private readonly B5CommandState? _state;
        private readonly Exception? _failure;

        public StubReader(B5CommandState state) => _state = state;

        public StubReader(Exception failure) => _failure = failure;

        public ValueTask<B5CommandState> ReadAsync(
            TR2Endpoint endpoint,
            CancellationToken cancellationToken = default)
        {
            if (_failure is not null)
            {
                throw _failure;
            }

            return ValueTask.FromResult(_state!);
        }
    }

    private sealed class ReservationStore : ICommandTransactionReservationStore
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
