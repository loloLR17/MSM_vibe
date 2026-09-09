namespace TR2.Supervision.Service;

public sealed class RuntimeReadinessGate
{
    public bool IsReady { get; private set; }

    public void EnsureReady()
    {
        if (!IsReady)
        {
            throw new InvalidOperationException("The supervision runtime is not ready for operational work.");
        }
    }

    internal void MarkReady()
    {
        if (IsReady)
        {
            throw new InvalidOperationException("The supervision runtime is already ready.");
        }

        IsReady = true;
    }

    internal void MarkNotReady()
    {
        IsReady = false;
    }
}
