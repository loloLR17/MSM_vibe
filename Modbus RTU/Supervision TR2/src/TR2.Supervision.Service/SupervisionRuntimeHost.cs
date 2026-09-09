namespace TR2.Supervision.Service;

public enum SupervisionRuntimeState
{
    Created,
    Starting,
    Running,
    Stopping,
    Stopped,
    Faulted
}

public sealed class SupervisionRuntimeHost
{
    private readonly SupervisionRuntimeComposition _composition;
    private readonly SupervisionRuntimeStartup _startup;
    private readonly ISupervisionRuntimeLoop _runtimeLoop;
    private bool _runInvoked;

    public SupervisionRuntimeHost(
        SupervisionRuntimeComposition composition,
        ISupervisionRuntimeLoop? runtimeLoop = null)
    {
        _composition = composition ?? throw new ArgumentNullException(nameof(composition));
        _startup = new SupervisionRuntimeStartup(composition);
        _runtimeLoop = runtimeLoop ?? IdleSupervisionRuntimeLoop.Instance;
    }

    public SupervisionRuntimeState State { get; private set; } = SupervisionRuntimeState.Created;

    public async Task RunAsync(CancellationToken cancellationToken = default)
    {
        if (_runInvoked)
        {
            throw new InvalidOperationException("The supervision runtime host is one-shot.");
        }

        _runInvoked = true;
        State = SupervisionRuntimeState.Starting;

        try
        {
            await _startup.StartAsync(cancellationToken);
            State = SupervisionRuntimeState.Running;

            try
            {
                await _runtimeLoop.RunAsync(cancellationToken);
            }
            catch (OperationCanceledException) when (cancellationToken.IsCancellationRequested)
            {
                // Requested host shutdown.
            }

            State = SupervisionRuntimeState.Stopping;
            _composition.ReadinessGate.MarkNotReady();
            State = SupervisionRuntimeState.Stopped;
        }
        catch (OperationCanceledException) when (cancellationToken.IsCancellationRequested)
        {
            _composition.ReadinessGate.MarkNotReady();
            State = SupervisionRuntimeState.Stopped;
        }
        catch
        {
            _composition.ReadinessGate.MarkNotReady();
            State = SupervisionRuntimeState.Faulted;
            throw;
        }
    }
}
