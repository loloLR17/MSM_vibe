using System.Globalization;
using TR2.Application;
using TR2.Domain;

namespace TR2.Persistence.Sqlite;

public sealed class SqliteCommandTransactionJournal : ICommandTransactionJournal
{
    private readonly SqliteDatabase _database;

    public SqliteCommandTransactionJournal(SqliteDatabase database)
    {
        _database = database ?? throw new ArgumentNullException(nameof(database));
    }

    public ValueTask AppendAsync(
        CommandTransactionJournalEvent entry,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(entry);
        cancellationToken.ThrowIfCancellationRequested();

        using var connection = _database.OpenConnection();
        using var transaction = connection.BeginTransaction();
        using var command = connection.CreateCommand();
        command.Transaction = transaction;
        command.CommandText = """
            INSERT INTO b5_transaction_journal(
                device_id,
                transaction_id,
                request_identity,
                event_kind,
                observed_utc)
            VALUES (
                $device_id,
                $transaction_id,
                $request_identity,
                $event_kind,
                $observed_utc);
            """;
        command.Parameters.AddWithValue("$device_id", (long)entry.DeviceId.Value);
        command.Parameters.AddWithValue("$transaction_id", (int)entry.TransactionId.Value);
        command.Parameters.AddWithValue("$request_identity", entry.RequestIdentity);
        command.Parameters.AddWithValue("$event_kind", entry.Kind.ToString());
        command.Parameters.AddWithValue(
            "$observed_utc",
            entry.ObservedAt.ToUniversalTime().ToString("O", CultureInfo.InvariantCulture));
        command.ExecuteNonQuery();
        transaction.Commit();

        return ValueTask.CompletedTask;
    }

    public ValueTask<IReadOnlyList<CommandTransactionJournalEvent>> ReadAsync(
        DeviceId deviceId,
        CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        using var connection = _database.OpenConnection();
        using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT
                device_id,
                transaction_id,
                request_identity,
                event_kind,
                observed_utc
            FROM b5_transaction_journal
            WHERE device_id = $device_id
            ORDER BY journal_id ASC;
            """;
        command.Parameters.AddWithValue("$device_id", (long)deviceId.Value);

        using var reader = command.ExecuteReader();
        var entries = new List<CommandTransactionJournalEvent>();
        while (reader.Read())
        {
            var persistedDeviceId = new DeviceId(checked((uint)reader.GetInt64(0)));
            var transactionId = new TransactionId(checked((ushort)reader.GetInt32(1)));
            var requestIdentity = reader.GetString(2);
            var kindText = reader.GetString(3);
            if (!Enum.TryParse<CommandTransactionJournalEventKind>(kindText, ignoreCase: false, out var kind)
                || !Enum.IsDefined(kind))
            {
                throw new InvalidDataException($"Unknown persisted B5 journal event kind '{kindText}'.");
            }

            var observedAt = DateTimeOffset.ParseExact(
                reader.GetString(4),
                "O",
                CultureInfo.InvariantCulture,
                DateTimeStyles.RoundtripKind);

            entries.Add(new CommandTransactionJournalEvent(
                persistedDeviceId,
                transactionId,
                requestIdentity,
                kind,
                observedAt));
        }

        return ValueTask.FromResult<IReadOnlyList<CommandTransactionJournalEvent>>(entries);
    }

    public ValueTask<IReadOnlyList<DeviceId>> ReadDeviceIdsAsync(
        CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        using var connection = _database.OpenConnection();
        using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT DISTINCT device_id
            FROM b5_transaction_journal
            ORDER BY device_id ASC;
            """;

        using var reader = command.ExecuteReader();
        var deviceIds = new List<DeviceId>();
        while (reader.Read())
        {
            deviceIds.Add(new DeviceId(checked((uint)reader.GetInt64(0))));
        }

        return ValueTask.FromResult<IReadOnlyList<DeviceId>>(deviceIds);
    }
}
