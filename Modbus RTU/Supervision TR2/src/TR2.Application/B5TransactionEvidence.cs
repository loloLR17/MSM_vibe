using TR2.Protocol;

namespace TR2.Application;

public static class B5TransactionEvidence
{
    private const ushort StatusSucceeded = 4;
    private const ushort StatusRefused = 5;
    private const ushort StatusFailed = 6;
    private const ushort StatusUnknownCommand = 7;
    private const ushort StatusNotAllowed = 8;

    public static bool HasTerminalEvidence(
        TransactionId transactionId,
        B5CommandState observed)
    {
        ArgumentNullException.ThrowIfNull(observed);

        var value = transactionId.Value;

        return (observed.LastTransactionId == value && IsTerminalStatus(observed.LastStatusFinal))
            || (observed.ActiveTransactionId == value && IsTerminalStatus(observed.Status));
    }

    public static bool IsTerminalStatus(ushort status) =>
        status is StatusSucceeded
            or StatusRefused
            or StatusFailed
            or StatusUnknownCommand
            or StatusNotAllowed;
}
