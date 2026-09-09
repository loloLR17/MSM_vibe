using TR2.Domain;
using Xunit;

namespace TR2.Application.Tests;

public sealed class CommandCoordinatorTests
{
    [Fact]
    public async Task First_transaction_is_one_and_is_persisted_before_becoming_active()
    {
        var store = new FakeReservationStore();
        var coordinator = new CommandCoordinator(new DeviceId(1001), store);

        var prepared = await coordinator.PrepareAsync("START_ACQUISITION");

        Assert.Equal((ushort)1, prepared.TransactionId.Value);
        Assert.Equal(new TransactionId(1), store.LastPersisted);
        Assert.Same(prepared, coordinator.ActiveTransaction);
        Assert.Equal(new[] { "get", "persist:1" }, store.Calls);
    }

    [Fact]
    public async Task Allocation_is_monotone_and_lifetime_strict()
    {
        var store = new FakeReservationStore(new TransactionId(41));
        var coordinator = new CommandCoordinator(new DeviceId(1001), store);

        var prepared = await coordinator.PrepareAsync("SELFTEST");
        coordinator.ResolveTerminal(prepared.TransactionId);
        var next = await coordinator.PrepareAsync("ACK_FAULT");

        Assert.Equal((ushort)42, prepared.TransactionId.Value);
        Assert.Equal((ushort)43, next.TransactionId.Value);
    }

    [Fact]
    public async Task A_nonterminal_transaction_blocks_new_commands()
    {
        var coordinator = new CommandCoordinator(new DeviceId(1001), new FakeReservationStore());
        await coordinator.PrepareAsync("START_ACQUISITION");

        await Assert.ThrowsAsync<InvalidOperationException>(async () =>
            await coordinator.PrepareAsync("STOP_ACQUISITION"));
    }

    [Fact]
    public async Task Timeout_after_submit_becomes_ambiguous_and_remains_blocking()
    {
        var coordinator = new CommandCoordinator(new DeviceId(1001), new FakeReservationStore());
        var prepared = await coordinator.PrepareAsync("APPLY_CONFIG");
        coordinator.MarkSubmitted();

        var ambiguous = coordinator.MarkAmbiguous();

        Assert.Equal(CommandTransactionState.Ambiguous, ambiguous.State);
        await Assert.ThrowsAsync<InvalidOperationException>(async () =>
            await coordinator.PrepareAsync("SYNC_TIME"));
        Assert.Equal(prepared.TransactionId, coordinator.ActiveTransaction!.TransactionId);
    }

    [Fact]
    public async Task Terminal_reconciliation_releases_active_slot_but_not_transaction_id()
    {
        var store = new FakeReservationStore();
        var coordinator = new CommandCoordinator(new DeviceId(1001), store);
        var first = await coordinator.PrepareAsync("APPLY_CONFIG");
        coordinator.MarkSubmitted();
        coordinator.MarkAmbiguous();

        coordinator.ResolveTerminal(first.TransactionId);
        var second = await coordinator.PrepareAsync("SYNC_TIME");

        Assert.Equal((ushort)2, second.TransactionId.Value);
    }

    [Fact]
    public async Task Transaction_id_space_exhaustion_is_explicit_and_does_not_wrap()
    {
        var store = new FakeReservationStore(new TransactionId(ushort.MaxValue));
        var coordinator = new CommandCoordinator(new DeviceId(1001), store);

        await Assert.ThrowsAsync<InvalidOperationException>(async () =>
            await coordinator.PrepareAsync("SELFTEST"));

        Assert.Null(coordinator.ActiveTransaction);
        Assert.Equal(0, store.PersistCount);
    }

    [Fact]
    public async Task Persistence_failure_does_not_publish_an_active_transaction()
    {
        var store = new FakeReservationStore { FailPersist = true };
        var coordinator = new CommandCoordinator(new DeviceId(1001), store);

        await Assert.ThrowsAsync<IOException>(async () =>
            await coordinator.PrepareAsync("SOFTWARE_RESET"));

        Assert.Null(coordinator.ActiveTransaction);
    }

    [Fact]
    public void Transaction_id_zero_is_rejected()
    {
        Assert.Throws<ArgumentOutOfRangeException>(() => new TransactionId(0));
    }

    private sealed class FakeReservationStore : ICommandTransactionReservationStore
    {
        private TransactionId? _lastAllocated;

        public FakeReservationStore(TransactionId? lastAllocated = null)
        {
            _lastAllocated = lastAllocated;
        }

        public List<string> Calls { get; } = [];

        public TransactionId? LastPersisted { get; private set; }

        public int PersistCount { get; private set; }

        public bool FailPersist { get; init; }

        public ValueTask<TransactionId?> GetLastAllocatedAsync(
            DeviceId deviceId,
            CancellationToken cancellationToken = default)
        {
            Calls.Add("get");
            return ValueTask.FromResult(_lastAllocated);
        }

        public ValueTask PersistAllocationAsync(
            DeviceId deviceId,
            TransactionId transactionId,
            CancellationToken cancellationToken = default)
        {
            Calls.Add($"persist:{transactionId.Value}");
            PersistCount++;

            if (FailPersist)
            {
                throw new IOException("Injected persistence failure.");
            }

            _lastAllocated = transactionId;
            LastPersisted = transactionId;
            return ValueTask.CompletedTask;
        }
    }
}
