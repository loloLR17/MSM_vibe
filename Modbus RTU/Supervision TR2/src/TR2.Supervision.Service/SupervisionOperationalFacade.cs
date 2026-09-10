using System.Collections.Concurrent;
using TR2.Application;
using TR2.Domain;
using TR2.Protocol;

namespace TR2.Supervision.Service;

public sealed record B5CommandIntent(
    ushort Code,
    ushort Param1,
    ushort Param2,
    uint Param3,
    ushort ConfirmKey)
{
    public B5CommandRequest Bind(TransactionId transactionId) =>
        new(Code, transactionId.Value, Param1, Param2, Param3, ConfirmKey);
}

public sealed record QueuedB5Command(
    ScheduledBusWork Work,
    DeviceId DeviceId,
    B5CommandRequest Request);

public sealed record QueuedB6CampaignSelection(
    ScheduledBusWork Work,
    ushort CampaignIndex);

public sealed class SupervisionOperationalFacade
{
    private readonly SupervisionRuntimeComposition _composition;
    private readonly FleetRefreshPlanner _refreshPlanner;
    private readonly object _contextSync = new();
    private readonly ConcurrentDictionary<DeviceId, SemaphoreSlim> _commandSubmissionGates = new();
    private readonly Dictionary<long, B5CommandRequest> _commandRequests = [];
    private readonly Dictionary<long, ScheduledBlockRefresh> _refreshes = [];
    private readonly Dictionary<long, ushort> _campaignSelections = [];

    public SupervisionOperationalFacade(SupervisionRuntimeComposition composition)
    {
        _composition = composition ?? throw new ArgumentNullException(nameof(composition));
        _refreshPlanner = new FleetRefreshPlanner(composition.BusWorkScheduler);
    }

    public async ValueTask<QueuedB5Command> QueueCommandAsync(
        TR2Endpoint endpoint,
        string requestIdentity,
        B5CommandIntent intent,
        DateTimeOffset dueAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(endpoint);
        ArgumentNullException.ThrowIfNull(intent);
        ArgumentException.ThrowIfNullOrWhiteSpace(requestIdentity);

        _composition.ReadinessGate.EnsureReady();
        var initialSession = RequireCompatibleSession(endpoint);
        var deviceId = initialSession.Device!.DeviceId;
        var gate = _commandSubmissionGates.GetOrAdd(deviceId, static _ => new SemaphoreSlim(1, 1));

        await gate.WaitAsync(cancellationToken).ConfigureAwait(false);
        try
        {
            _composition.ReadinessGate.EnsureReady();
            var currentSession = RequireCompatibleSession(endpoint);
            if (currentSession.Device!.DeviceId != deviceId)
            {
                throw new InvalidOperationException(
                    "The endpoint device_id changed while the B5 command request was waiting for submission serialization.");
            }

            var coordinator = GetOrCreateCoordinator(deviceId);
            await coordinator.PrepareAsync(requestIdentity, dueAt, cancellationToken).ConfigureAwait(false);

            var transaction = coordinator.ActiveTransaction
                ?? throw new InvalidOperationException("B5 command preparation did not create an active transaction.");
            var request = intent.Bind(transaction.TransactionId);

            var work = _composition.BusWorkScheduler.QueuePriority(
                endpoint,
                BusWorkKind.CommandTransaction,
                dueAt,
                queuedWork =>
                {
                    lock (_contextSync)
                    {
                        _commandRequests.Add(queuedWork.WorkId, request);
                    }
                });

            return new QueuedB5Command(work, deviceId, request);
        }
        finally
        {
            gate.Release();
        }
    }

    public QueuedB6CampaignSelection QueueCampaignSelection(
        TR2Endpoint endpoint,
        ushort campaignIndex,
        DateTimeOffset dueAt)
    {
        ArgumentNullException.ThrowIfNull(endpoint);

        _composition.ReadinessGate.EnsureReady();
        RequireCompatibleSession(endpoint);

        var work = _composition.BusWorkScheduler.QueuePriority(
            endpoint,
            BusWorkKind.CampaignSelection,
            dueAt,
            queuedWork =>
            {
                lock (_contextSync)
                {
                    _campaignSelections.Add(queuedWork.WorkId, campaignIndex);
                }
            });

        return new QueuedB6CampaignSelection(work, campaignIndex);
    }

    public IReadOnlyList<ScheduledBlockRefresh> QueuePostReconnectRefresh(
        TR2Endpoint endpoint,
        DateTimeOffset dueAt)
    {
        ArgumentNullException.ThrowIfNull(endpoint);

        _composition.ReadinessGate.EnsureReady();
        var session = RequireCompatibleSession(endpoint);
        var refreshes = _refreshPlanner.QueuePostReconnectRefresh(session, dueAt);

        lock (_contextSync)
        {
            foreach (var refresh in refreshes)
            {
                _refreshes.Add(refresh.Work.WorkId, refresh);
            }
        }

        return refreshes;
    }

    public B5CommandRequest GetCommandRequest(ScheduledBusWork work)
    {
        ArgumentNullException.ThrowIfNull(work);
        if (work.Kind != BusWorkKind.CommandTransaction)
        {
            throw new InvalidOperationException("The work item is not a B5 command transaction.");
        }

        lock (_contextSync)
        {
            return _commandRequests.TryGetValue(work.WorkId, out var request)
                ? request
                : throw new KeyNotFoundException("No B5 command request is registered for this work item.");
        }
    }

    public ushort GetCampaignSelection(ScheduledBusWork work)
    {
        ArgumentNullException.ThrowIfNull(work);
        if (work.Kind != BusWorkKind.CampaignSelection)
        {
            throw new InvalidOperationException("The work item is not a B6 campaign selection.");
        }

        lock (_contextSync)
        {
            return _campaignSelections.TryGetValue(work.WorkId, out var campaignIndex)
                ? campaignIndex
                : throw new KeyNotFoundException("No B6 campaign selection is registered for this work item.");
        }
    }

    public ScheduledBlockRefresh GetRefresh(ScheduledBusWork work)
    {
        ArgumentNullException.ThrowIfNull(work);
        if (work.Kind != BusWorkKind.ExplicitRefresh)
        {
            throw new InvalidOperationException("The work item is not an explicit refresh.");
        }

        lock (_contextSync)
        {
            return _refreshes.TryGetValue(work.WorkId, out var refresh)
                ? refresh
                : throw new KeyNotFoundException("No explicit refresh context is registered for this work item.");
        }
    }

    private TR2Session RequireCompatibleSession(TR2Endpoint endpoint)
    {
        var session = _composition.FleetRegistry.GetSession(endpoint);
        if (session.State != TR2SessionState.Compatible || session.Device is null)
        {
            throw new InvalidOperationException(
                "Operational B5, B6 and explicit refresh work requires a compatible identified TR2 session.");
        }

        return session;
    }

    private CommandCoordinator GetOrCreateCoordinator(DeviceId deviceId) =>
        _composition.CommandCoordinatorRegistry.GetOrAdd(
            deviceId,
            id => new CommandCoordinator(
                id,
                _composition.CommandReservationStore,
                _composition.CommandJournal));
}
