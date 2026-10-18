namespace TR2.Application;

// Reception-PC history, not window UTC or a source of current telemetry.
public sealed record LiveHistoryQuery(string Bus, byte Unit, DateTimeOffset From, DateTimeOffset To,
    int Limit = 500, long? Before = null)
{
    public void Validate()
    {
        if (string.IsNullOrWhiteSpace(Bus) || Bus.Length > 128 || Unit is < 1 or > 247 ||
            From >= To || To - From > TimeSpan.FromDays(7) || Limit is < 1 or > 500 || Before is <= 0)
            throw new ArgumentException("Bus 1..128 caractères, adresse 1..247, période positive ≤7 jours, limite 1..500 et curseur positif requis.");
    }
}
public sealed record LiveHistoryEntry(long RowId, LiveObservation Observation);
public sealed record LiveHistoryPage(IReadOnlyList<LiveHistoryEntry> Entries, long? NextBefore,
    int ScannedRows, string Coverage, bool Historical = true, bool IsCurrent = false);
public interface ILiveHistoryReadSource
{
    ValueTask<LiveHistoryPage> ReadHistoryAsync(LiveHistoryQuery query, CancellationToken ct = default);
}
