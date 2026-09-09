using System.Globalization;
using TR2.Application;
using TR2.Domain;

namespace TR2.Persistence.Sqlite;

public sealed class SqliteCommandTransactionReservationStore : ICommandTransactionReservationStore
{
    private readonly SqliteDatabase _database;

    public SqliteCommandTransactionReservationStore(SqliteDatabase database)
    {
        _database = database ?? throw new ArgumentNullException(nameof(database));
    }

    public ValueTask<TransactionId?> GetLastAllocatedAsync(
        DeviceId deviceId,
        CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        using var connection = _database.OpenConnection();
        using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT transaction_id
            FROM b5_transaction_reservation
            WHERE device_id = $device_id;
            """;
        command.Parameters.AddWithValue("$device_id", (long)deviceId.Value);

        var scalar = command.ExecuteScalar();
        if (scalar is null || scalar is DBNull)
        {
            return ValueTask.FromResult<TransactionId?>(null);
        }

        var value = checked((ushort)Convert.ToInt32(scalar, CultureInfo.InvariantCulture));
        return ValueTask.FromResult<TransactionId?>(new TransactionId(value));
    }

    public ValueTask PersistAllocationAsync(
        DeviceId deviceId,
        TransactionId transactionId,
        CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        using var connection = _database.OpenConnection();
        using var transaction = connection.BeginTransaction();
        using var command = connection.CreateCommand();
        command.Transaction = transaction;
        command.CommandText = """
            INSERT INTO b5_transaction_reservation(device_id, transaction_id, persisted_utc)
            VALUES ($device_id, $transaction_id, $persisted_utc)
            ON CONFLICT(device_id) DO UPDATE SET
                transaction_id = excluded.transaction_id,
                persisted_utc = excluded.persisted_utc;
            """;
        command.Parameters.AddWithValue("$device_id", (long)deviceId.Value);
        command.Parameters.AddWithValue("$transaction_id", (int)transactionId.Value);
        command.Parameters.AddWithValue(
            "$persisted_utc",
            DateTimeOffset.UtcNow.ToString("O", CultureInfo.InvariantCulture));
        command.ExecuteNonQuery();
        transaction.Commit();

        return ValueTask.CompletedTask;
    }
}
