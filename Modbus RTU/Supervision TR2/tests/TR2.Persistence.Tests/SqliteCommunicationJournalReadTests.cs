using TR2.Application;
using TR2.Domain;
using TR2.Persistence.Sqlite;
using Xunit;

namespace TR2.Persistence.Tests;

public sealed class SqliteCommunicationJournalReadTests : IDisposable
{
    private readonly string _directory = Path.Combine(
        Path.GetTempPath(),
        $"tr2-communication-read-{Guid.NewGuid():N}");

    [Fact]
    public async Task ReadLatestReturnsNewestPersistedFailuresWithoutChangingTaxonomy()
    {
        var databasePath = Path.Combine(_directory, "communication-read.db");
        var database = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        var sink = new SqliteCommunicationJournalSink(database);
        var endpoint = new TR2Endpoint(new SerialBus("bus-a"), new ModbusAddress(5));
        var start = new DateTimeOffset(2026, 9, 9, 16, 0, 0, TimeSpan.Zero);

        await sink.AppendAsync(new CommunicationFailureEvent(
            endpoint,
            null,
            CommunicationOperation.Polling,
            CommunicationFailureCategory.Timeout,
            start,
            "System.TimeoutException",
            "first"));
        await sink.AppendAsync(new CommunicationFailureEvent(
            endpoint,
            new DeviceId(42),
            CommunicationOperation.CommandTransaction,
            CommunicationFailureCategory.Io,
            start.AddSeconds(1),
            "System.IO.IOException",
            "second"));
        await sink.AppendAsync(new CommunicationFailureEvent(
            endpoint,
            new DeviceId(42),
            CommunicationOperation.ExplicitRefresh,
            CommunicationFailureCategory.ModbusExceptionResponse,
            start.AddSeconds(2),
            "NModbus.SlaveException",
            "third"));

        var failures = sink.ReadLatest(2);

        Assert.Collection(
            failures,
            failure =>
            {
                Assert.Equal("bus-a", failure.BusId);
                Assert.Equal((byte)5, failure.ModbusAddress);
                Assert.Equal((uint)42, failure.DeviceId);
                Assert.Equal(CommunicationOperation.ExplicitRefresh, failure.Operation);
                Assert.Equal(CommunicationFailureCategory.ModbusExceptionResponse, failure.Category);
                Assert.Equal(start.AddSeconds(2), failure.ObservedAt);
                Assert.Equal("NModbus.SlaveException", failure.ExceptionType);
                Assert.Equal("third", failure.Message);
            },
            failure =>
            {
                Assert.Equal(CommunicationOperation.CommandTransaction, failure.Operation);
                Assert.Equal(CommunicationFailureCategory.Io, failure.Category);
                Assert.Equal("second", failure.Message);
            });
    }

    [Fact]
    public void ReadLatestRejectsNonPositiveLimit()
    {
        var database = new SqliteDatabase(
            new SqlitePersistenceOptions(Path.Combine(_directory, "invalid-limit.db")));
        var sink = new SqliteCommunicationJournalSink(database);

        Assert.Throws<ArgumentOutOfRangeException>(() => sink.ReadLatest(0));
        Assert.Throws<ArgumentOutOfRangeException>(() => sink.ReadLatest(-1));
    }

    public void Dispose()
    {
        Microsoft.Data.Sqlite.SqliteConnection.ClearAllPools();
        if (Directory.Exists(_directory))
        {
            Directory.Delete(_directory, recursive: true);
        }
    }
}
