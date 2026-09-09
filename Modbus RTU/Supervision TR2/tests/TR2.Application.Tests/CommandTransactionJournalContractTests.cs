using TR2.Domain;
using Xunit;

namespace TR2.Application.Tests;

public sealed class CommandTransactionJournalContractTests
{
    private static readonly DateTimeOffset ObservedAt =
        new(2026, 9, 9, 10, 0, 0, TimeSpan.Zero);

    [Theory]
    [InlineData(CommandTransactionJournalEventKind.Prepared)]
    [InlineData(CommandTransactionJournalEventKind.Submitted)]
    [InlineData(CommandTransactionJournalEventKind.Ambiguous)]
    [InlineData(CommandTransactionJournalEventKind.TerminalEvidenceObserved)]
    public void Journal_event_is_structured_and_indexed_by_device_id(
        CommandTransactionJournalEventKind kind)
    {
        var entry = new CommandTransactionJournalEvent(
            new DeviceId(1234),
            new TransactionId(42),
            "APPLY_CONFIG:cfg-7",
            kind,
            ObservedAt);

        Assert.Equal(new DeviceId(1234), entry.DeviceId);
        Assert.Equal(new TransactionId(42), entry.TransactionId);
        Assert.Equal("APPLY_CONFIG:cfg-7", entry.RequestIdentity);
        Assert.Equal(kind, entry.Kind);
        Assert.Equal(ObservedAt, entry.ObservedAt);
    }

    [Fact]
    public void Journal_event_requires_request_identity()
    {
        Assert.Throws<ArgumentException>(() =>
            new CommandTransactionJournalEvent(
                new DeviceId(1234),
                new TransactionId(42),
                " ",
                CommandTransactionJournalEventKind.Prepared,
                ObservedAt));
    }

    [Fact]
    public async Task Journal_contract_reads_events_by_device_id()
    {
        ICommandTransactionJournal journal = new RecordingJournal();
        var deviceId = new DeviceId(1234);
        var entry = new CommandTransactionJournalEvent(
            deviceId,
            new TransactionId(42),
            "SYNC_TIME:1",
            CommandTransactionJournalEventKind.Prepared,
            ObservedAt);

        await journal.AppendAsync(entry);
        var entries = await journal.ReadAsync(deviceId);

        Assert.Single(entries);
        Assert.Same(entry, entries[0]);
    }

    private sealed class RecordingJournal : ICommandTransactionJournal
    {
        private readonly List<CommandTransactionJournalEvent> _entries = [];

        public ValueTask AppendAsync(
            CommandTransactionJournalEvent entry,
            CancellationToken cancellationToken = default)
        {
            _entries.Add(entry);
            return ValueTask.CompletedTask;
        }

        public ValueTask<IReadOnlyList<CommandTransactionJournalEvent>> ReadAsync(
            DeviceId deviceId,
            CancellationToken cancellationToken = default)
        {
            IReadOnlyList<CommandTransactionJournalEvent> result = _entries
                .Where(entry => entry.DeviceId == deviceId)
                .ToArray();

            return ValueTask.FromResult(result);
        }
    }
}
