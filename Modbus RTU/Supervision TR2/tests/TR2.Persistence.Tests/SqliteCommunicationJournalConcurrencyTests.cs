using Microsoft.Data.Sqlite;
using TR2.Application;
using TR2.Domain;
using TR2.Persistence.Sqlite;
using Xunit;

namespace TR2.Persistence.Tests;

public sealed class SqliteCommunicationJournalConcurrencyTests : IDisposable
{
    private readonly string _directory = Path.Combine(
        Path.GetTempPath(),
        $"tr2-communication-concurrency-{Guid.NewGuid():N}");

    [Fact]
    public async Task ConcurrentWritersAndReaderPreserveAllJournalRowsAndDatabaseIntegrity()
    {
        const int writerCount = 4;
        const int writesPerWriter = 50;
        const int expectedCount = writerCount * writesPerWriter;

        var databasePath = Path.Combine(_directory, "communication-concurrency.db");
        var database = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        var sink = new SqliteCommunicationJournalSink(database);

        // Initialize schema before contention so this test exercises normal runtime
        // concurrency rather than concurrent migration startup.
        using (database.OpenConnection())
        {
        }

        using var safetyCancellation = new CancellationTokenSource(TimeSpan.FromSeconds(30));
        var writersRemaining = writerCount;
        var successfulReads = 0;
        var startedAt = new DateTimeOffset(2026, 9, 9, 20, 0, 0, TimeSpan.Zero);

        var writers = Enumerable.Range(0, writerCount)
            .Select(writerIndex => Task.Run(async () =>
            {
                try
                {
                    for (var sequence = 0; sequence < writesPerWriter; sequence++)
                    {
                        safetyCancellation.Token.ThrowIfCancellationRequested();
                        var ordinal = writerIndex * writesPerWriter + sequence;
                        var endpoint = new TR2Endpoint(
                            new SerialBus($"rs485-concurrent-{writerIndex}"),
                            new ModbusAddress((byte)(writerIndex + 1)));

                        await sink.AppendAsync(
                            new CommunicationFailureEvent(
                                endpoint,
                                new DeviceId((uint)(5000 + writerIndex)),
                                CommunicationOperation.Polling,
                                CommunicationFailureCategory.Timeout,
                                startedAt.AddMilliseconds(ordinal),
                                "ConcurrentStressException",
                                $"writer-{writerIndex}-sequence-{sequence}"),
                            safetyCancellation.Token);
                    }
                }
                finally
                {
                    Interlocked.Decrement(ref writersRemaining);
                }
            }, safetyCancellation.Token))
            .ToArray();

        var reader = Task.Run(async () =>
        {
            while (Volatile.Read(ref writersRemaining) > 0)
            {
                safetyCancellation.Token.ThrowIfCancellationRequested();
                var recent = sink.ReadLatest(10);
                Assert.InRange(recent.Count, 0, 10);
                Interlocked.Increment(ref successfulReads);
                await Task.Delay(1, safetyCancellation.Token);
            }

            var finalRecent = sink.ReadLatest(10);
            Assert.Equal(10, finalRecent.Count);
            Interlocked.Increment(ref successfulReads);
        }, safetyCancellation.Token);

        await Task.WhenAll(writers.Append(reader));

        Assert.True(successfulReads > 0);

        SqliteConnection.ClearAllPools();

        var reopened = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        using var connection = reopened.OpenConnection();

        Assert.Equal(
            expectedCount,
            ReadInt32(connection, "SELECT COUNT(*) FROM communication_failure_journal;"));
        Assert.Equal(
            expectedCount,
            ReadInt32(connection, "SELECT COUNT(DISTINCT message) FROM communication_failure_journal;"));
        Assert.Equal(
            writerCount,
            ReadInt32(connection, "SELECT COUNT(DISTINCT bus_id) FROM communication_failure_journal;"));

        for (var writerIndex = 0; writerIndex < writerCount; writerIndex++)
        {
            using var command = connection.CreateCommand();
            command.CommandText = """
                SELECT COUNT(*)
                FROM communication_failure_journal
                WHERE bus_id = $bus_id;
                """;
            command.Parameters.AddWithValue("$bus_id", $"rs485-concurrent-{writerIndex}");
            Assert.Equal(writesPerWriter, Convert.ToInt32(command.ExecuteScalar()));
        }

        using var integrityCommand = connection.CreateCommand();
        integrityCommand.CommandText = "PRAGMA integrity_check;";
        Assert.Equal(
            "ok",
            Convert.ToString(
                integrityCommand.ExecuteScalar(),
                System.Globalization.CultureInfo.InvariantCulture));
    }

    public void Dispose()
    {
        SqliteConnection.ClearAllPools();

        if (Directory.Exists(_directory))
        {
            Directory.Delete(_directory, recursive: true);
        }
    }

    private static int ReadInt32(SqliteConnection connection, string sql)
    {
        using var command = connection.CreateCommand();
        command.CommandText = sql;
        return Convert.ToInt32(
            command.ExecuteScalar(),
            System.Globalization.CultureInfo.InvariantCulture);
    }
}
