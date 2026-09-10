using TR2.Application;
using TR2.Domain;
using TR2.Supervision.Web;

namespace TR2.Supervision.Service;

public sealed class RuntimeCommandSink : ISupervisionCommandSink, ISupervisionCampaignSelectionSink, ISupervisionCommandHistoryReadSource
{
    private const ushort ProtectedCommandConfirmKey = 0xA55A;
    private const int CommandHistoryLimit = 20;
    private readonly PhysicalSupervisionRuntime _runtime;
    private readonly SemaphoreSlim _queueGate = new(1, 1);

    public RuntimeCommandSink(PhysicalSupervisionRuntime runtime)
    {
        _runtime = runtime ?? throw new ArgumentNullException(nameof(runtime));
    }

    public async ValueTask<IhmCommandQueueResult> QueueAsync(
        uint deviceId,
        string requestIdentity,
        IhmB5CommandSubmission submission,
        DateTimeOffset requestedAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(requestIdentity);
        ArgumentNullException.ThrowIfNull(submission);

        await _queueGate.WaitAsync(cancellationToken);
        try
        {
            var id = new DeviceId(deviceId);
            var existingEntries = await _runtime.Composition.CommandJournal.ReadAsync(id, cancellationToken);
            if (existingEntries.Any(entry => string.Equals(entry.RequestIdentity, requestIdentity, StringComparison.Ordinal)))
            {
                return new IhmCommandQueueResult(
                    IhmCommandQueueStatus.DuplicateRequestIdentity,
                    Detail: "requestIdentity was already used for this device.");
            }

            var sessionResult = ResolveSession(deviceId);
            if (sessionResult.Session is null)
            {
                return new IhmCommandQueueResult(sessionResult.Status switch
                {
                    SessionResolutionStatus.NotFound => IhmCommandQueueStatus.DeviceNotFound,
                    _ => IhmCommandQueueStatus.Conflict
                }, Detail: sessionResult.Detail);
            }

            if (!_runtime.Composition.ReadinessGate.IsReady)
            {
                return new IhmCommandQueueResult(
                    IhmCommandQueueStatus.NotReady,
                    Detail: "The supervision runtime is not ready.");
            }

            try
            {
                var queued = await _runtime.Operations.QueueCommandAsync(
                    sessionResult.Session.Endpoint,
                    requestIdentity,
                    MapIntent(submission),
                    requestedAt,
                    cancellationToken);

                return new IhmCommandQueueResult(
                    IhmCommandQueueStatus.Accepted,
                    queued.Work.WorkId,
                    queued.DeviceId.Value,
                    queued.Request.TransactionId,
                    null);
            }
            catch (InvalidOperationException)
            {
                return new IhmCommandQueueResult(
                    IhmCommandQueueStatus.Conflict,
                    Detail: "A B5 command cannot be queued in the current transaction state.");
            }
        }
        finally
        {
            _queueGate.Release();
        }
    }

    public async ValueTask<IhmCampaignSelectionQueueResult> QueueCampaignSelectionAsync(
        uint deviceId,
        ushort campaignIndex,
        DateTimeOffset requestedAt,
        CancellationToken cancellationToken = default)
    {
        await _queueGate.WaitAsync(cancellationToken);
        try
        {
            var sessionResult = ResolveSession(deviceId);
            if (sessionResult.Session is null)
            {
                return new IhmCampaignSelectionQueueResult(sessionResult.Status switch
                {
                    SessionResolutionStatus.NotFound => IhmCampaignSelectionQueueStatus.DeviceNotFound,
                    _ => IhmCampaignSelectionQueueStatus.Conflict
                }, Detail: sessionResult.Detail);
            }

            if (!_runtime.Composition.ReadinessGate.IsReady)
            {
                return new IhmCampaignSelectionQueueResult(
                    IhmCampaignSelectionQueueStatus.NotReady,
                    Detail: "The supervision runtime is not ready.");
            }

            try
            {
                var queued = _runtime.Operations.QueueCampaignSelection(
                    sessionResult.Session.Endpoint,
                    campaignIndex,
                    requestedAt);

                return new IhmCampaignSelectionQueueResult(
                    IhmCampaignSelectionQueueStatus.Accepted,
                    queued.Work.WorkId,
                    deviceId,
                    queued.CampaignIndex,
                    null);
            }
            catch (InvalidOperationException)
            {
                return new IhmCampaignSelectionQueueResult(
                    IhmCampaignSelectionQueueStatus.Conflict,
                    Detail: "The B6 campaign selection cannot be queued in the current runtime state.");
            }
        }
        finally
        {
            _queueGate.Release();
        }
    }

    public async ValueTask<IReadOnlyList<IhmB5TransactionReadModel>> ReadAsync(
        uint deviceId,
        CancellationToken cancellationToken = default)
    {
        var id = new DeviceId(deviceId);
        var entries = await _runtime.Composition.CommandJournal.ReadAsync(id, cancellationToken);
        _runtime.Composition.CommandCoordinatorRegistry.TryGet(id, out var coordinator);
        var active = coordinator?.ActiveTransaction;

        return entries
            .GroupBy(entry => new { TransactionId = entry.TransactionId.Value, entry.RequestIdentity })
            .Select(group => group.OrderBy(entry => entry.ObservedAt).Last())
            .OrderByDescending(entry => entry.ObservedAt)
            .Take(CommandHistoryLimit)
            .Select(entry => new IhmB5TransactionReadModel(
                entry.DeviceId.Value,
                entry.TransactionId.Value,
                entry.RequestIdentity,
                ProjectState(entry, active),
                entry.ObservedAt))
            .ToArray();
    }

    private static IhmB5TransactionState ProjectState(
        CommandTransactionJournalEvent entry,
        CommandTransaction? active)
    {
        if (active is { State: CommandTransactionState.Ambiguous }
            && active.TransactionId == entry.TransactionId
            && string.Equals(active.RequestIdentity, entry.RequestIdentity, StringComparison.Ordinal))
        {
            // Recovery deliberately does not append a synthetic journal event. The IHM must
            // nevertheless expose the current blocking runtime state reconstructed by S5.
            return IhmB5TransactionState.Ambiguous;
        }

        return entry.Kind switch
        {
            CommandTransactionJournalEventKind.Prepared => IhmB5TransactionState.Prepared,
            CommandTransactionJournalEventKind.Submitted => IhmB5TransactionState.Submitted,
            CommandTransactionJournalEventKind.Ambiguous => IhmB5TransactionState.Ambiguous,
            CommandTransactionJournalEventKind.TerminalEvidenceObserved => IhmB5TransactionState.TerminalEvidenceObserved,
            _ => throw new InvalidDataException($"Unsupported B5 journal event kind '{entry.Kind}'.")
        };
    }

    private SessionResolution ResolveSession(uint deviceId)
    {
        var sessions = _runtime.Composition.FleetRegistry.Sessions
            .Where(session => session.Device?.DeviceId.Value == deviceId)
            .ToArray();

        if (sessions.Length == 0)
        {
            return new SessionResolution(SessionResolutionStatus.NotFound, null, null);
        }

        if (sessions.Length != 1 || sessions[0].State != TR2SessionState.Compatible || sessions[0].Device is null)
        {
            return new SessionResolution(
                SessionResolutionStatus.Conflict,
                null,
                "The device does not currently have one compatible identified session.");
        }

        return new SessionResolution(SessionResolutionStatus.Resolved, sessions[0], null);
    }

    private static B5CommandIntent MapIntent(IhmB5CommandSubmission submission) => submission.Command switch
    {
        IhmB5Command.ApplyConfig => new B5CommandIntent(1, 0, 0, 0, 0),
        IhmB5Command.SyncTime => new B5CommandIntent(2, 0, 0, 0, 0),
        IhmB5Command.StartAcquisition => new B5CommandIntent(3, 0, 0, 0, 0),
        IhmB5Command.StopAcquisition => new B5CommandIntent(4, 0, 0, 0, 0),
        IhmB5Command.Selftest => new B5CommandIntent(5, 0, 0, 0, 0),
        IhmB5Command.AcknowledgeFault when submission.AcknowledgeAll == false && submission.FaultCode.HasValue =>
            new B5CommandIntent(6, submission.FaultCode.Value, 0, 0, 0),
        IhmB5Command.AcknowledgeFault when submission.AcknowledgeAll == true && !submission.FaultCode.HasValue =>
            new B5CommandIntent(6, 0, 1, 0, 0),
        IhmB5Command.RefreshIndicators => new B5CommandIntent(7, 0, 0, 0, 0),
        IhmB5Command.EnterMaintenance => new B5CommandIntent(8, 0, 0, 0, 0),
        IhmB5Command.ExitMaintenance => new B5CommandIntent(9, 0, 0, 0, 0),
        IhmB5Command.SoftwareReset when submission.ConfirmProtectedCommand =>
            new B5CommandIntent(10, 0, 0, 0, ProtectedCommandConfirmKey),
        _ => throw new InvalidOperationException("The B5 command submission is not valid for its command contract.")
    };

    private enum SessionResolutionStatus
    {
        Resolved,
        NotFound,
        Conflict
    }

    private sealed record SessionResolution(
        SessionResolutionStatus Status,
        TR2Session? Session,
        string? Detail);
}
