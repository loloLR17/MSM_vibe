using TR2.Application;
using TR2.Domain;

namespace TR2.Supervision.Service;

public sealed record PollingWorkExecutionResult(bool EndpointCompatible);

public interface IPollingWorkRunner
{
    ValueTask<PollingWorkExecutionResult> ExecuteAsync(
        ScheduledBusWork work,
        DateTimeOffset observedAt,
        CancellationToken cancellationToken = default);
}

public interface IPriorityWorkRunner
{
    ValueTask ExecuteAsync(
        ScheduledBusWork work,
        DateTimeOffset observedAt,
        CancellationToken cancellationToken = default);
}

public interface ISupervisionRuntimeLoop
{
    Task RunAsync(CancellationToken cancellationToken = default);
}

internal sealed class IdleSupervisionRuntimeLoop : ISupervisionRuntimeLoop
{
    public static IdleSupervisionRuntimeLoop Instance { get; } = new();

    private IdleSupervisionRuntimeLoop()
    {
    }

    public Task RunAsync(CancellationToken cancellationToken = default) =>
        Task.Delay(Timeout.InfiniteTimeSpan, cancellationToken);
}

internal sealed class RejectingPriorityWorkRunner : IPriorityWorkRunner
{
    public static RejectingPriorityWorkRunner Instance { get; } = new();

    private RejectingPriorityWorkRunner()
    {
    }

    public ValueTask ExecuteAsync(
        ScheduledBusWork work,
        DateTimeOffset observedAt,
        CancellationToken cancellationToken = default) =>
        throw new InvalidOperationException(
            "Priority work is pending but no S3-F priority work runner is configured.");
}

public sealed class SupervisionPollingLoop : ISupervisionRuntimeLoop
{
    private readonly SupervisionRuntimeComposition _composition;
    private readonly IPollingWorkRunner _pollingRunner;
    private readonly IPriorityWorkRunner _priorityRunner;
    private readonly TimeProvider _timeProvider;
    private readonly SerialBusReconnectScheduler? _reconnectScheduler;
    private readonly Dictionary<TR2Endpoint, EndpointPollingState> _states = [];
    private bool _runInvoked;

    public SupervisionPollingLoop(
        SupervisionRuntimeComposition composition,
        IPollingWorkRunner runner,
        TimeProvider? timeProvider = null)
        : this(composition, runner, RejectingPriorityWorkRunner.Instance, timeProvider)
    {
    }

    public SupervisionPollingLoop(
        SupervisionRuntimeComposition composition,
        IPollingWorkRunner pollingRunner,
        IPriorityWorkRunner priorityRunner,
        TimeProvider? timeProvider = null)
    {
        _composition = composition ?? throw new ArgumentNullException(nameof(composition));
        _pollingRunner = pollingRunner ?? throw new ArgumentNullException(nameof(pollingRunner));
        _priorityRunner = priorityRunner ?? throw new ArgumentNullException(nameof(priorityRunner));
        _timeProvider = timeProvider ?? TimeProvider.System;

        if (composition.Configuration.Reconnect is not null)
        {
            _reconnectScheduler = new SerialBusReconnectScheduler(
                composition,
                composition.Configuration.Reconnect.Interval);
        }

        foreach (var bus in composition.Configuration.Buses)
        {
            foreach (var endpoint in bus.Endpoints)
            {
                _states.Add(endpoint, new EndpointPollingState());
            }
        }
    }

    public async Task RunAsync(CancellationToken cancellationToken = default)
    {
        if (_runInvoked)
        {
            throw new InvalidOperationException("The supervision polling loop is one-shot.");
        }

        _runInvoked = true;
        _composition.ReadinessGate.EnsureReady();
        cancellationToken.ThrowIfCancellationRequested();

        var startedAt = _timeProvider.GetUtcNow();
        foreach (var state in _states.Values)
        {
            state.StaticDueAt = startedAt;
        }

        while (true)
        {
            cancellationToken.ThrowIfCancellationRequested();
            var now = _timeProvider.GetUtcNow();

            if (_reconnectScheduler is not null)
            {
                await _reconnectScheduler.RunDueAsync(now, cancellationToken);
            }

            QueueDuePolling(now);

            foreach (var configuredBus in _composition.Configuration.Buses)
            {
                cancellationToken.ThrowIfCancellationRequested();

                var work = _composition.BusWorkScheduler.BeginNext(configuredBus.Bus, now);
                if (work is null)
                {
                    continue;
                }

                if (work.Kind == BusWorkKind.Polling)
                {
                    await ExecutePollingAsync(work, now, cancellationToken);
                }
                else
                {
                    await _priorityRunner.ExecuteAsync(work, now, cancellationToken);
                }
            }

            await Task.Delay(
                _composition.Configuration.Polling.ScanInterval,
                _timeProvider,
                cancellationToken);
        }
    }

    private async ValueTask ExecutePollingAsync(
        ScheduledBusWork work,
        DateTimeOffset observedAt,
        CancellationToken cancellationToken)
    {
        if (work.PollingGroup is null)
        {
            throw new InvalidOperationException("Polling work has no polling group.");
        }

        var state = _states[work.Endpoint];
        PollingWorkExecutionResult result;
        try
        {
            result = await _pollingRunner.ExecuteAsync(work, observedAt, cancellationToken);
        }
        finally
        {
            state.IsQueued = false;
        }

        UpdateScheduleAfterExecution(
            state,
            work.PollingGroup.Value,
            result,
            _timeProvider.GetUtcNow());
    }

    private void QueueDuePolling(DateTimeOffset now)
    {
        foreach (var (endpoint, state) in _states)
        {
            if (state.IsQueued)
            {
                continue;
            }

            var session = _composition.FleetRegistry.GetSession(endpoint);
            var group = session.State == TR2SessionState.Compatible
                ? SelectDueOperationalGroup(state, now)
                : state.StaticDueAt <= now ? PollingGroup.Static : null;

            if (group is null)
            {
                continue;
            }

            var dueAt = GetDueAt(state, group.Value);
            _composition.BusWorkScheduler.QueuePolling(endpoint, group.Value, dueAt);
            state.IsQueued = true;
        }
    }

    private static PollingGroup? SelectDueOperationalGroup(
        EndpointPollingState state,
        DateTimeOffset now)
    {
        var candidates = new[]
        {
            (Group: PollingGroup.Fast, DueAt: state.FastDueAt),
            (Group: PollingGroup.Medium, DueAt: state.MediumDueAt),
            (Group: PollingGroup.Slow, DueAt: state.SlowDueAt)
        };

        return candidates
            .Where(candidate => candidate.DueAt is not null && candidate.DueAt <= now)
            .OrderBy(candidate => candidate.DueAt)
            .ThenBy(candidate => candidate.Group)
            .Select(candidate => (PollingGroup?)candidate.Group)
            .FirstOrDefault();
    }

    private void UpdateScheduleAfterExecution(
        EndpointPollingState state,
        PollingGroup group,
        PollingWorkExecutionResult result,
        DateTimeOffset completedAt)
    {
        var policy = _composition.Configuration.Polling;

        switch (group)
        {
            case PollingGroup.Static:
                state.StaticDueAt = completedAt + policy.StaticRetryInterval;
                if (result.EndpointCompatible)
                {
                    state.FastDueAt = completedAt;
                    state.MediumDueAt = completedAt;
                    state.SlowDueAt = completedAt;
                }
                break;

            case PollingGroup.Fast:
                state.FastDueAt = completedAt + policy.FastInterval;
                break;

            case PollingGroup.Medium:
                state.MediumDueAt = completedAt + policy.MediumInterval;
                break;

            case PollingGroup.Slow:
                state.SlowDueAt = completedAt + policy.SlowInterval;
                break;

            default:
                throw new ArgumentOutOfRangeException(nameof(group));
        }

        if (!result.EndpointCompatible)
        {
            state.StaticDueAt = completedAt + policy.StaticRetryInterval;
        }
    }

    private static DateTimeOffset GetDueAt(EndpointPollingState state, PollingGroup group) => group switch
    {
        PollingGroup.Static => state.StaticDueAt,
        PollingGroup.Fast => state.FastDueAt ?? throw new InvalidOperationException("FAST polling has no due time."),
        PollingGroup.Medium => state.MediumDueAt ?? throw new InvalidOperationException("MEDIUM polling has no due time."),
        PollingGroup.Slow => state.SlowDueAt ?? throw new InvalidOperationException("SLOW polling has no due time."),
        _ => throw new ArgumentOutOfRangeException(nameof(group))
    };

    private sealed class EndpointPollingState
    {
        public bool IsQueued { get; set; }
        public DateTimeOffset StaticDueAt { get; set; }
        public DateTimeOffset? FastDueAt { get; set; }
        public DateTimeOffset? MediumDueAt { get; set; }
        public DateTimeOffset? SlowDueAt { get; set; }
    }
}
