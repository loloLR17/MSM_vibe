using TR2.Domain;
using Xunit;

namespace TR2.Application.Tests;

public sealed class CommandCoordinatorJournalIntegrationTests
{
    private static readonly DateTimeOffset T0 =
        new(2026, 9, 9, 10, 0, 0, TimeSpan.Zero);

    [Fact]
    public async Task Durable_transitions_are_journaled_before_local_state_is_published()
    {
        var store = new InMemoryReservationStore();
        var journal = new RecordingJournal();
        var coordinator = new CommandCoordinator(new DeviceId(1001), store, journal);

        var prepared = await coordinator.PrepareAsync("START", T0);
        Assert.Equal(CommandTransactionState.Prepared, coordinator.ActiveTransaction?.State);

        var submitted = await coordinator.MarkSubmittedAsync(T0.AddSeconds(1));
        Assert.Equal(CommandTransactionState.Submitted, submitted.State);

        var ambiguous = await coordinator.MarkAmbiguousAsync(T0.AddSeconds(2));
        Assert.Equal(CommandTransactionState.Ambiguous, ambiguous.State);

        await coordinator.ResolveTerminalAsync(ambiguous.TransactionId, T0.AddSeconds(3));
        Assert.Null(coordinator.ActiveTransaction);

        Assert.Equal(
            new[]
            {
                CommandTransactionJournalEventKind.Prepared,
                CommandTransactionJournalEventKind.Submitted,
                CommandTransactionJournalEventKind.Ambiguous,
                CommandTransactionJournalEventKind.TerminalEvidenceObserved
            },
            journal.Events.Select(entry => entry.Kind));
        Assert.All(journal.Events, entry => Assert.Equal(prepared.TransactionId, entry.TransactionId));
        Assert.All(journal.Events, entry => Assert.Equal("START", entry.RequestIdentity));
    }

    [Fact]
    public async Task Journal_failure_does_not_publish_submitted_transition()
    {
        var journal = new RecordingJournal();
        var coordinator = new CommandCoordinator(
            new DeviceId(1001),
            new InMemoryReservationStore(),
            journal);
        await coordinator.PrepareAsync("START", T0);
        journal.FailNextAppend = true;

        await Assert.ThrowsAsync<IOException>(async () =>
            await coordinator.MarkSubmittedAsync(T0.AddSeconds(1)));

        Assert.Equal(CommandTransactionState.Prepared, coordinator.ActiveTransaction?.State);
    }

    [Fact]
    public async Task Journal_failure_after_allocation_keeps_transaction_id_consumed_but_not_active()
    {
        var store = new InMemoryReservationStore();
        var journal = new RecordingJournal { FailNextAppend = true };
        var coordinator = new CommandCoordinator(new DeviceId(1001), store, journal);

        await Assert.ThrowsAsync<IOException>(async () =>
            await coordinator.PrepareAsync("START", T0));

        Assert.Null(coordinator.ActiveTransaction);
        Assert.Equal(new TransactionId(1), await store.GetLastAllocatedAsync(new DeviceId(1001)));
    }

    [Fact]
    public async Task Orchestrator_async_start_journals_submitted_transition()
    {
        var scheduler = new BusWorkScheduler();
        var orchestrator = new CommandBusOrchestrator(scheduler);
        var journal = new RecordingJournal();
        var coordinator = new CommandCoordinator(
            new DeviceId(1001),
            new InMemoryReservationStore(),
            journal);
        var endpoint = new TR2Endpoint(new SerialBus("RS485-A"), new ModbusAddress(10));

        var work = await orchestrator.PrepareAndQueueAsync(endpoint, coordinator, "START", T0);
        var started = await orchestrator.BeginNextAsync(endpoint.Bus, T0.AddSeconds(1));

        Assert.Equal(work, started);
        Assert.Equal(CommandTransactionState.Submitted, coordinator.ActiveTransaction?.State);
        Assert.Equal(
            new[]
            {
                CommandTransactionJournalEventKind.Prepared,
                CommandTransactionJournalEventKind.Submitted
            },
            journal.Events.Select(entry => entry.Kind));
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

    private sealed class RecordingJournal : ICommandTransactionJournal
    {
        public List<CommandTransactionJournalEvent> Events { get; } = [];

        public bool FailNextAppend { get; set; }

        public ValueTask AppendAsync(
            CommandTransactionJournalEvent entry,
            CancellationToken cancellationToken = default)
        {
            if (FailNextAppend)
            {
                FailNextAppend = false;
                throw new IOException("Injected journal persistence failure.");
            }

            Events.Add(entry);
            return ValueTask.CompletedTask;
        }

        public ValueTask<IReadOnlyList<CommandTransactionJournalEvent>> ReadAsync(
            DeviceId deviceId,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromResult<IReadOnlyList<CommandTransactionJournalEvent>>(
                Events.Where(entry => entry.DeviceId == deviceId).ToArray());
    }
}
