using TR2.Domain;
using TR2.Protocol;
using Xunit;

namespace TR2.Application.Tests;

public sealed class B5CommandExecutionServiceTests
{
    private static readonly DateTimeOffset Now =
        new(2026, 9, 9, 11, 0, 0, TimeSpan.Zero);

    [Fact]
    public async Task Successful_submit_publishes_submitted_only_after_protocol_submit()
    {
        var writer = new RecordingWriter();
        var service = new B5CommandExecutionService(writer);
        var coordinator = await PreparedCoordinatorAsync();

        await service.ExecuteAsync(Endpoint(), coordinator, Request(1), Now);

        Assert.Equal(new[] { "prepare", "submit" }, writer.Calls);
        Assert.Equal(CommandTransactionState.Submitted, coordinator.ActiveTransaction?.State);
    }

    [Fact]
    public async Task Preparation_failure_leaves_transaction_prepared_and_does_not_attempt_submit()
    {
        var writer = new RecordingWriter { FailPrepare = true };
        var service = new B5CommandExecutionService(writer);
        var coordinator = await PreparedCoordinatorAsync();

        await Assert.ThrowsAsync<IOException>(async () =>
            await service.ExecuteAsync(Endpoint(), coordinator, Request(1), Now));

        Assert.Equal(new[] { "prepare" }, writer.Calls);
        Assert.Equal(CommandTransactionState.Prepared, coordinator.ActiveTransaction?.State);
    }

    [Fact]
    public async Task Submit_write_failure_marks_transaction_ambiguous_without_retry()
    {
        var writer = new RecordingWriter { FailSubmit = true };
        var service = new B5CommandExecutionService(writer);
        var coordinator = await PreparedCoordinatorAsync();

        await Assert.ThrowsAsync<IOException>(async () =>
            await service.ExecuteAsync(Endpoint(), coordinator, Request(1), Now));

        Assert.Equal(new[] { "prepare", "submit" }, writer.Calls);
        Assert.Equal(CommandTransactionState.Ambiguous, coordinator.ActiveTransaction?.State);
    }

    [Fact]
    public async Task Request_transaction_id_must_match_active_supervision_transaction()
    {
        var writer = new RecordingWriter();
        var service = new B5CommandExecutionService(writer);
        var coordinator = await PreparedCoordinatorAsync();

        await Assert.ThrowsAsync<InvalidOperationException>(async () =>
            await service.ExecuteAsync(Endpoint(), coordinator, Request(2), Now));

        Assert.Empty(writer.Calls);
        Assert.Equal(CommandTransactionState.Prepared, coordinator.ActiveTransaction?.State);
    }

    [Fact]
    public async Task Submit_failure_can_be_journaled_directly_from_prepared_as_ambiguous()
    {
        var journal = new RecordingJournal();
        var coordinator = new CommandCoordinator(
            new DeviceId(1001),
            new ReservationStore(),
            journal);
        await coordinator.PrepareAsync("START", Now.AddSeconds(-1));
        var service = new B5CommandExecutionService(new RecordingWriter { FailSubmit = true });

        await Assert.ThrowsAsync<IOException>(async () =>
            await service.ExecuteAsync(Endpoint(), coordinator, Request(1), Now));

        Assert.Equal(
            new[]
            {
                CommandTransactionJournalEventKind.Prepared,
                CommandTransactionJournalEventKind.Ambiguous
            },
            journal.Events.Select(entry => entry.Kind));
        Assert.Equal(CommandTransactionState.Ambiguous, coordinator.ActiveTransaction?.State);
    }

    private static async Task<CommandCoordinator> PreparedCoordinatorAsync()
    {
        var coordinator = new CommandCoordinator(new DeviceId(1001), new ReservationStore());
        await coordinator.PrepareAsync("START");
        return coordinator;
    }

    private static TR2Endpoint Endpoint() =>
        new(new SerialBus("RS485-A"), new ModbusAddress(10));

    private static B5CommandRequest Request(ushort transactionId) =>
        new(3, transactionId, 0, 0, 0, 0);

    private sealed class RecordingWriter : IB5CommandWriter
    {
        public List<string> Calls { get; } = [];
        public bool FailPrepare { get; init; }
        public bool FailSubmit { get; init; }

        public ValueTask PrepareAsync(
            TR2Endpoint endpoint,
            B5CommandRequest request,
            CancellationToken cancellationToken = default)
        {
            Calls.Add("prepare");
            return FailPrepare
                ? ValueTask.FromException(new IOException("Injected preparation failure."))
                : ValueTask.CompletedTask;
        }

        public ValueTask SubmitAsync(
            TR2Endpoint endpoint,
            CancellationToken cancellationToken = default)
        {
            Calls.Add("submit");
            return FailSubmit
                ? ValueTask.FromException(new IOException("Injected submit failure."))
                : ValueTask.CompletedTask;
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

    private sealed class RecordingJournal : ICommandTransactionJournal
    {
        public List<CommandTransactionJournalEvent> Events { get; } = [];

        public ValueTask AppendAsync(
            CommandTransactionJournalEvent entry,
            CancellationToken cancellationToken = default)
        {
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
