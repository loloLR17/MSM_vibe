using TR2.Domain;

namespace TR2.Application;

public sealed class CommandCoordinator
{
    private readonly ICommandTransactionReservationStore _store;

    public CommandCoordinator(DeviceId deviceId, ICommandTransactionReservationStore store)
    {
        ArgumentNullException.ThrowIfNull(store);
        DeviceId = deviceId;
        _store = store;
    }

    public DeviceId DeviceId { get; }

    public CommandTransaction? ActiveTransaction { get; private set; }

    public async ValueTask<CommandTransaction> PrepareAsync(
        string requestIdentity,
        CancellationToken cancellationToken = default)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(requestIdentity);

        if (ActiveTransaction is not null)
        {
            throw new InvalidOperationException("A nonterminal transaction is already active for this device.");
        }

        var lastAllocated = await _store.GetLastAllocatedAsync(DeviceId, cancellationToken);
        if (lastAllocated is { Value.Value: ushort.MaxValue })
        {
            throw new InvalidOperationException("The lifetime-strict transaction_id space is exhausted for this device.");
        }

        var nextValue = lastAllocated is null
            ? (ushort)1
            : checked((ushort)(lastAllocated.Value.Value + 1));
        var transactionId = new TransactionId(nextValue);

        await _store.PersistAllocationAsync(DeviceId, transactionId, cancellationToken);

        var prepared = new CommandTransaction(
            DeviceId,
            transactionId,
            requestIdentity,
            CommandTransactionState.Prepared);

        ActiveTransaction = prepared;
        return prepared;
    }

    public CommandTransaction MarkSubmitted()
    {
        var active = RequireActive();
        ActiveTransaction = active.MarkSubmitted();
        return ActiveTransaction;
    }

    public CommandTransaction MarkAmbiguous()
    {
        var active = RequireActive();
        ActiveTransaction = active.MarkAmbiguous();
        return ActiveTransaction;
    }

    public void ResolveTerminal(TransactionId transactionId)
    {
        var active = RequireActive();
        if (active.TransactionId != transactionId)
        {
            throw new InvalidOperationException("Terminal evidence does not match the active transaction.");
        }

        ActiveTransaction = null;
    }

    private CommandTransaction RequireActive() =>
        ActiveTransaction
        ?? throw new InvalidOperationException("No active command transaction exists for this device.");
}
