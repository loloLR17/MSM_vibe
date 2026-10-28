using TR2.Domain;
using TR2.Protocol;
using TR2.Transport;

namespace TR2.Application;

public enum LiveContinuity { SegmentStart, Sequential, RepeatedSequence, Gap, IndeterminateSaturation, RegressedSequence, CommunicationLost, Rejected }
public sealed record LiveObservation(Guid ObservationId, DeviceId? DeviceId, Guid SegmentId, string BusId, byte ModbusAddress,
    DateTimeOffset ReceivedAt, LiveProfileIdentity? Identity, LiveB3Snapshot? Snapshot,
    LiveContinuity Continuity, string Source, string? Detail)
{
    // Not present on B3: deliberately unknown, never inferred from B5 or a campaign.
    public IReadOnlyList<ushort>? RejectedB3Registers { get; init; }
    public B3VibrationSupervision? RejectedB3Value { get; init; }
    public ulong? RunId => null;
    public uint? Epoch => null;
    public ulong? ConfigurationId => null;
    public uint? ConfigurationGeneration => null;
    public ulong? EventMonotonicMicroseconds => null;
    public LiveAc1Snapshot? DynamicAcceleration { get; init; }
    public string Mode => "LIVE_READONLY";
}
public interface ILiveObservationSink
{
    ValueTask AppendAsync(LiveObservation observation, CancellationToken ct = default);
}
public sealed record LiveReadView(LiveObservation Observation, bool TransportConnected,
    bool ReceptionStale, bool IsCurrent, string ArchiveState, string? ArchiveError);
public interface ILiveReadSource
{
    IReadOnlyList<LiveReadView> Read(DateTimeOffset now);
}

// One cycle at a time, including persistence: observers cannot reorder device/segment history.
public sealed class LiveObservationService : ILiveReadSource
{
    private readonly LiveProfileReader _reader;
    private readonly IRegisterTransport _transport;
    private readonly bool _ac1Enabled;
    private readonly ILiveObservationSink _sink;
    private readonly TimeProvider _clock;
    private readonly TimeSpan _staleAfter;
    private readonly string _source;
    private readonly SemaphoreSlim _cycle = new(1, 1);
    private readonly Dictionary<TR2Endpoint, LiveReadView> _latest = new();
    private readonly object _gate = new();

    public LiveObservationService(IRegisterTransport transport, ILiveObservationSink sink,
        bool allowDiagnostic = false, TimeProvider? clock = null, TimeSpan? staleAfter = null,
        string source = "RegisterTransport", bool ac1Enabled = false)
    {
        _reader = new LiveProfileReader(transport, allowDiagnostic);
        _transport = transport; _ac1Enabled = ac1Enabled;
        _sink = sink ?? throw new ArgumentNullException(nameof(sink));
        _clock = clock ?? TimeProvider.System;
        _staleAfter = staleAfter ?? TimeSpan.FromSeconds(5);
        if (_staleAfter <= TimeSpan.Zero) throw new ArgumentOutOfRangeException(nameof(staleAfter));
        ArgumentException.ThrowIfNullOrWhiteSpace(source);
        _source = source;
    }

    public async ValueTask<LiveReadView> PollAsync(TR2Endpoint endpoint, CancellationToken ct = default)
    {
        await _cycle.WaitAsync(ct);
        try
        {
            LiveReadView? previous;
            lock (_gate) _latest.TryGetValue(endpoint, out previous);
            LiveProfileIdentity? identity = null;
            LiveB3Snapshot? snapshot = null;
            LiveAc1Snapshot? dynamic = null;
            string? error = null;
            LiveB3ReadException? rejectedB3 = null;
            var continuity = LiveContinuity.SegmentStart;
            var segment = previous?.Observation.SegmentId ?? Guid.NewGuid();
            var connected = true;
            try
            {
                // Re-identify on each cycle, never carry identity across a changed endpoint.
                identity = await _reader.IdentifyAsync(endpoint, ct);
                snapshot = await _reader.ReadAsync(endpoint, identity, ct);
                if (_ac1Enabled)
                {
                    if (identity.Profile != SupervisionProfile.LiveDiagnostic) throw new NotSupportedException("AC1 operator diagnostic profile required.");
                    dynamic = LiveAc1Snapshot.Parse(await _transport.ReadRegistersAsync(endpoint.Bus.Id, endpoint.Address.Value, LiveAc1Snapshot.StartAddress, LiveAc1Snapshot.RegisterCount, ct));
                }
                if (previous?.TransportConnected == true && previous.Observation.Identity == identity && previous.Observation.Snapshot is { } old)
                {
                    var before = old.Value.CalculationSequence;
                    var after = snapshot.Value.CalculationSequence;
                    continuity = after < before ? LiveContinuity.RegressedSequence :
                        after == uint.MaxValue ? LiveContinuity.IndeterminateSaturation :
                        after == before ? LiveContinuity.RepeatedSequence :
                        after - before == 1 ? LiveContinuity.Sequential : LiveContinuity.Gap;
                    if (continuity == LiveContinuity.RegressedSequence) segment = Guid.NewGuid();
                }
                else segment = Guid.NewGuid();
            }
            catch (OperationCanceledException) when (ct.IsCancellationRequested) { throw; }
            catch (LiveB3ReadException ex)
            {
                continuity = LiveContinuity.Rejected;
                rejectedB3 = ex;
                // Transport responded, but the observation must never become a validated snapshot.
                error = ex.Message;
            }
            catch (Exception ex) when (ex is InvalidDataException or NotSupportedException)
            {
                continuity = LiveContinuity.Rejected;
                connected = false;
                error = ex.Message;
            }
            catch (Exception ex) when (ex is IOException or TimeoutException or ModbusTransportFailureException)
            {
                continuity = LiveContinuity.CommunicationLost;
                connected = false;
                error = ex.Message;
                // Identity is usable only after this cycle's B0, not from a past session.
            }
            var observation = new LiveObservation(Guid.NewGuid(), identity?.DeviceId, segment, endpoint.Bus.Id, endpoint.Address.Value,
                _clock.GetUtcNow(), identity, snapshot, continuity, _source, error)
            { RejectedB3Registers = rejectedB3?.Registers, RejectedB3Value = rejectedB3?.Decoded, DynamicAcceleration = dynamic };
            var archiveState = "Persisted";
            string? archiveError = null;
            try { await _sink.AppendAsync(observation, ct); }
            catch (OperationCanceledException) when (ct.IsCancellationRequested) { throw; }
            catch (Exception ex) { archiveState = "Failed"; archiveError = ex.Message; }
            var view = new LiveReadView(observation, connected, false,
                connected && snapshot is { ValuesValid: true, Fresh: true, Held: false }, archiveState, archiveError);
            lock (_gate) _latest[endpoint] = view;
            return view;
        }
        finally { _cycle.Release(); }
    }

    public IReadOnlyList<LiveReadView> Read(DateTimeOffset now)
    {
        lock (_gate)
            return _latest.Values.Select(v =>
            {
                var stale = now < v.Observation.ReceivedAt || now - v.Observation.ReceivedAt > _staleAfter;
                return v with { ReceptionStale = stale, IsCurrent = v.IsCurrent && !stale };
            }).ToArray();
    }
}
