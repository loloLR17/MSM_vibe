using TR2.Domain;

namespace TR2.Application;

public interface ICommandTransactionReservationStore
{
    ValueTask<TransactionId?> GetLastAllocatedAsync(
        DeviceId deviceId,
        CancellationToken cancellationToken = default);

    ValueTask PersistAllocationAsync(
        DeviceId deviceId,
        TransactionId transactionId,
        CancellationToken cancellationToken = default);
}
