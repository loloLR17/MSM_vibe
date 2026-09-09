using System.Globalization;
using TR2.Application;

namespace TR2.Persistence.Sqlite;

public sealed record PersistedCommunicationFailure(
    long FailureId,
    string BusId,
    byte ModbusAddress,
    uint? DeviceId,
    CommunicationOperation Operation,
    CommunicationFailureCategory Category,
    DateTimeOffset ObservedAt,
    string ExceptionType,
    string Message);

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
                category,
                observed_utc,
                exception_type,
                message)
            VALUES (
                $bus_id,
                $modbus_address,
                $device_id,
                $operation,
                $category,
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
        command.Parameters.AddWithValue("$category", failure.Category.ToString());
        command.Parameters.AddWithValue(
            "$observed_utc",
            failure.ObservedAt.ToUniversalTime().ToString("O", CultureInfo.InvariantCulture));
        command.Parameters.AddWithValue("$exception_type", failure.ExceptionType);
        command.Parameters.AddWithValue("$message", failure.Message);

        command.ExecuteNonQuery();
        transaction.Commit();
        return ValueTask.CompletedTask;
    }

    public IReadOnlyList<PersistedCommunicationFailure> ReadLatest(int limit)
    {
        if (limit <= 0)
        {
            throw new ArgumentOutOfRangeException(nameof(limit));
        }

        using var connection = _database.OpenConnection();
        using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT
                failure_id,
                bus_id,
                modbus_address,
                device_id,
                operation,
                category,
                observed_utc,
                exception_type,
                message
            FROM communication_failure_journal
            ORDER BY failure_id DESC
            LIMIT $limit;
            """;
        command.Parameters.AddWithValue("$limit", limit);

        using var reader = command.ExecuteReader();
        var failures = new List<PersistedCommunicationFailure>();
        while (reader.Read())
        {
            failures.Add(new PersistedCommunicationFailure(
                reader.GetInt64(0),
                reader.GetString(1),
                checked((byte)reader.GetInt32(2)),
                reader.IsDBNull(3) ? null : checked((uint)reader.GetInt64(3)),
                Enum.Parse<CommunicationOperation>(reader.GetString(4), ignoreCase: false),
                Enum.Parse<CommunicationFailureCategory>(reader.GetString(5), ignoreCase: false),
                DateTimeOffset.Parse(
                    reader.GetString(6),
                    CultureInfo.InvariantCulture,
                    DateTimeStyles.RoundtripKind),
                reader.GetString(7),
                reader.GetString(8)));
        }

        return failures;
    }
}
