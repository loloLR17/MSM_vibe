using TR2.Domain;
using TR2.Protocol;

namespace TR2.Application;

public sealed class B5CommandExecutionService
{
    private readonly IB5CommandWriter _writer;

    public B5CommandExecutionService(IB5CommandWriter writer)
    {
        ArgumentNullException.ThrowIfNull(writer);
        _writer = writer;
    }

    public async ValueTask ExecuteAsync(
        TR2Endpoint endpoint,
        CommandCoordinator coordinator,
        B5CommandRequest request,
        DateTimeOffset observedAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(endpoint);
        ArgumentNullException.ThrowIfNull(coordinator);
        ArgumentNullException.ThrowIfNull(request);

        var transaction = coordinator.ActiveTransaction
            ?? throw new InvalidOperationException("B5 command execution requires an active supervision transaction.");

        if (transaction.State != CommandTransactionState.Prepared)
        {
            throw new InvalidOperationException("B5 command execution requires a prepared supervision transaction.");
        }

        if (request.TransactionId != transaction.TransactionId.Value)
        {
            throw new InvalidOperationException("B5 request transaction_id does not match the active supervision transaction.");
        }

        await _writer.PrepareAsync(endpoint, request, cancellationToken);

        try
        {
            await _writer.SubmitAsync(endpoint, cancellationToken);
        }
        catch
        {
            await coordinator.MarkAmbiguousAfterSubmitAttemptAsync(observedAt, cancellationToken);
            throw;
        }

        await coordinator.MarkSubmittedAsync(observedAt, cancellationToken);
    }
}
