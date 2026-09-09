using System.Globalization;
using TR2.Application;

namespace TR2.Persistence.Sqlite;

public sealed class SqliteCommunicationJournalSink : ICommunicationJournalSink
{
    private readonly SqliteDatabase _database;

    public SqliteCommunicationJournalSink(SqliteDatabase database)
    {
        _database = database ?? throw new ArgumentNullException(nameof(database));
    }

    public ValueTask AppendAsync(
        CommunicationFailureEvent failure,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(failure);
        cancellationToken.ThrowIfCancellationRequested();

        using var connection = _database.OpenConnection();
        using var transaction = connection.BeginTransaction();
        using var command = connection.CreateCommand();
        command.Transaction = transaction;
        command.CommandText = """
            INSERT INTO communication_failure_journal(
                bus_id,
                modbus_address,
                device_id,
                operation,
                observed_utc,
                exception_type,
                message)
            VALUES (
                $bus_id,
                $modbus_address,
                $device_id,
                $operation,
                $observed_utc,
                $exception_type,
                $message);
            """;

        command.Parameters.AddWithValue("$bus_id", failure.Endpoint.Bus.Id);
        command.Parameters.AddWithValue("$modbus_address", (int)failure.Endpoint.Address.Value);
        command.Parameters.AddWithValue(
            "$device_id",
            failure.DeviceId is null ? DBNull.Value : (long)failure.DeviceId.Value.Value);
        command.Parameters.AddWithValue("$operation", failure.Operation.ToString());
        command.Parameters.AddWithValue(
            "$observed_utc",
            failure.ObservedAt.ToUniversalTime().ToString("O", CultureInfo.InvariantCulture));
        command.Parameters.AddWithValue("$exception_type", failure.ExceptionType);
        command.Parameters.AddWithValue("$message", failure.Message);

        command.ExecuteNonQuery();
        transaction.Commit();
        return ValueTask.CompletedTask;
    }
}
