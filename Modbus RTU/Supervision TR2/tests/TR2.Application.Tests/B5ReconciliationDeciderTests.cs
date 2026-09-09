using TR2.Domain;
using TR2.Protocol;

namespace TR2.Application.Tests;

public sealed class B5ReconciliationDeciderTests
{
    private static readonly DeviceId DeviceId = new(0x12345678);
    private static readonly TransactionId TransactionId = new(42);

    [Fact]
    public void MatchingLastTerminalTransactionProvidesTerminalEvidence()
    {
        var decision = B5ReconciliationDecider.Decide(
            AmbiguousTransaction(),
            State(lastTransactionId: 42, lastStatusFinal: 4));

        Assert.Equal(B5ReconciliationOutcome.TerminalEvidence, decision.Outcome);
        Assert.Equal(TransactionId, decision.TransactionId);
    }

    [Theory]
    [InlineData(4)]
    [InlineData(5)]
    [InlineData(6)]
    [InlineData(7)]
    [InlineData(8)]
    public void MatchingActiveTerminalStatusProvidesTerminalEvidence(ushort status)
    {
        var decision = B5ReconciliationDecider.Decide(
            AmbiguousTransaction(),
            State(activeTransactionId: 42, status: status));

        Assert.Equal(B5ReconciliationOutcome.TerminalEvidence, decision.Outcome);
    }

    [Theory]
    [InlineData(0)]
    [InlineData(1)]
    [InlineData(2)]
    [InlineData(3)]
    [InlineData(9)]
    public void MatchingActiveNonterminalOrReservedStatusKeepsTransactionBlocked(ushort status)
    {
        var decision = B5ReconciliationDecider.Decide(
            AmbiguousTransaction(),
            State(activeTransactionId: 42, status: status));

        Assert.Equal(B5ReconciliationOutcome.StillNonTerminal, decision.Outcome);
    }

    [Fact]
    public void DifferentTransactionIdsAreInsufficientEvidence()
    {
        var decision = B5ReconciliationDecider.Decide(
            AmbiguousTransaction(),
            State(
                activeTransactionId: 41,
                status: 4,
                lastTransactionId: 43,
                lastStatusFinal: 4));

        Assert.Equal(B5ReconciliationOutcome.InsufficientEvidence, decision.Outcome);
    }

    [Fact]
    public void MatchingLastTransactionWithNonterminalOrReservedStatusIsNotTerminalEvidence()
    {
        var decision = B5ReconciliationDecider.Decide(
            AmbiguousTransaction(),
            State(lastTransactionId: 42, lastStatusFinal: 9));

        Assert.Equal(B5ReconciliationOutcome.InsufficientEvidence, decision.Outcome);
    }

    [Fact]
    public void NonAmbiguousSupervisionTransactionCannotBeReconciled()
    {
        var transaction = AmbiguousTransaction() with { State = CommandTransactionState.Submitted };

        Assert.Throws<InvalidOperationException>(() =>
            B5ReconciliationDecider.Decide(transaction, State()));
    }

    private static CommandTransaction AmbiguousTransaction() =>
        new(DeviceId, TransactionId, "START_ACQUISITION", CommandTransactionState.Ambiguous);

    private static B5CommandState State(
        ushort activeTransactionId = 0,
        ushort status = 0,
        ushort lastTransactionId = 0,
        ushort lastStatusFinal = 0) =>
        new(
            RequestCode: 0,
            RequestTransactionId: 0,
            RequestParam1: 0,
            RequestParam2: 0,
            RequestParam3: 0,
            RequestConfirmKey: 0,
            RequestControl: 0,
            ActiveCode: 0,
            ActiveTransactionId: activeTransactionId,
            Status: status,
            ResultCode: 0,
            ResultDetail: 0,
            EngineFlags: 0,
            LastCode: 0,
            LastTransactionId: lastTransactionId,
            LastStatusFinal: lastStatusFinal,
            LastResultCode: 0,
            LastTimestampSeconds: 0);
}
