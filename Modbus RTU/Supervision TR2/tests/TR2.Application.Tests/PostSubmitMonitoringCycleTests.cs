using TR2.Domain;
using TR2.Protocol;
using Xunit;

namespace TR2.Application.Tests;

public sealed class PostSubmitMonitoringCycleTests
{
    private static readonly DateTimeOffset T0 =
        new(2026, 9, 9, 13, 0, 0, TimeSpan.Zero);

    [Fact]
    public async Task Pending_observation_requeues_monitoring_with_same_timeout()
    {
        var scheduler = new BusWorkScheduler();
        var endpoint = Endpoint();
        var coordinator = await SubmittedCoordinatorAsync();
        var orchestrator = new CommandBusOrchestrator(
            scheduler,
            null,
            null,
            new B5PostSubmitMonitor(
                new StubReader(State(activeTransactionId: 1, status: 3))));

        var current = orchestrator.QueuePostSubmitMonitoring(
            endpoint,
            coordinator,
            T0,
            T0.AddSeconds(10));
        var active = orchestrator.BeginNext(endpoint.Bus, T0)!;

        var cycle = await orchestrator.ExecutePostSubmitMonitoringAndRequeueAsync(
            active,
            T0.AddSeconds(1),
            T0.AddSeconds(2));

        Assert.Equal(B5PostSubmitOutcome.Pending, cycle.Observation.Outcome);
        Assert.NotNull(cycle.NextMonitoringWork);
        Assert.Equal(BusWorkKind.CommandPostSubmitMonitoring, cycle.NextMonitoringWork!.Kind);
        Assert.Equal(T0.AddSeconds(2), cycle.NextMonitoringWork.DueAt);
        Assert.Equal(CommandTransactionState.Submitted, coordinator.ActiveTransaction?.State);
        Assert.Null(orchestrator.BeginNext(endpoint.Bus, T0.AddSeconds(1)));
        Assert.Equal(cycle.NextMonitoringWork, orchestrator.BeginNext(endpoint.Bus, T0.AddSeconds(2)));
    }

    [Fact]
    public async Task Terminal_observation_does_not_requeue()
    {
        var scheduler = new BusWorkScheduler();
        var endpoint = Endpoint();
        var coordinator = await SubmittedCoordinatorAsync();
        var orchestrator = new CommandBusOrchestrator(
            scheduler,
            null,
            null,
            new B5PostSubmitMonitor(
                new StubReader(State(lastTransactionId: 1, lastStatusFinal: 4))));

        var current = orchestrator.QueuePostSubmitMonitoring(
            endpoint,
            coordinator,
            T0,
            T0.AddSeconds(10));
        var active = orchestrator.BeginNext(endpoint.Bus, T0)!;

        var cycle = await orchestrator.ExecutePostSubmitMonitoringAndRequeueAsync(
            active,
            T0.AddSeconds(1),
            T0.AddSeconds(2));

        Assert.Equal(B5PostSubmitOutcome.TerminalEvidence, cycle.Observation.Outcome);
        Assert.Null(cycle.NextMonitoringWork);
        Assert.Null(coordinator.ActiveTransaction);
    }

    [Fact]
    public async Task Timeout_observation_does_not_requeue()
    {
        var scheduler = new BusWorkScheduler();
        var endpoint = Endpoint();
        var coordinator = await SubmittedCoordinatorAsync();
        var orchestrator = new CommandBusOrchestrator(
            scheduler,
            null,
            null,
            new B5PostSubmitMonitor(
                new StubReader(State(activeTransactionId: 1, status: 3))));

        var current = orchestrator.QueuePostSubmitMonitoring(
            endpoint,
            coordinator,
            T0,
            T0.AddSeconds(5));
        var active = orchestrator.BeginNext(endpoint.Bus, T0)!;

        var cycle = await orchestrator.ExecutePostSubmitMonitoringAndRequeueAsync(
            active,
            T0.AddSeconds(5),
            T0.AddSeconds(6));

        Assert.Equal(B5PostSubmitOutcome.TimedOutAmbiguous, cycle.Observation.Outcome);
        Assert.Null(cycle.NextMonitoringWork);
        Assert.Equal(CommandTransactionState.Ambiguous, coordinator.ActiveTransaction?.State);
    }

    [Fact]
    public async Task Read_failure_does_not_requeue_automatically()
    {
        var scheduler = new BusWorkScheduler();
        var endpoint = Endpoint();
        var coordinator = await SubmittedCoordinatorAsync();
        var orchestrator = new CommandBusOrchestrator(
            scheduler,
            null,
            null,
            new B5PostSubmitMonitor(new ThrowingReader()));

        var current = orchestrator.QueuePostSubmitMonitoring(
            endpoint,
            coordinator,
            T0,
            T0.AddSeconds(10));
        var active = orchestrator.BeginNext(endpoint.Bus, T0)!;

        await Assert.ThrowsAsync<IOException>(async () =>
            await orchestrator.ExecutePostSubmitMonitoringAndRequeueAsync(
                active,
                T0.AddSeconds(1),
                T0.AddSeconds(2)));

        Assert.Equal(CommandTransactionState.Submitted, coordinator.ActiveTransaction?.State);
        Assert.Null(orchestrator.BeginNext(endpoint.Bus, T0.AddSeconds(2)));
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
        private readonly B5CommandState _state;

        public StubReader(B5CommandState state) => _state = state;

        public ValueTask<B5CommandState> ReadAsync(
            TR2Endpoint endpoint,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromResult(_state);
    }

    private sealed class ThrowingReader : IB5CommandStateReader
    {
        public ValueTask<B5CommandState> ReadAsync(
            TR2Endpoint endpoint,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromException<B5CommandState>(new IOException("read failed"));
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
