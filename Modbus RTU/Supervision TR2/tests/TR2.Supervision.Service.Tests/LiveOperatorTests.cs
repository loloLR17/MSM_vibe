using TR2.Supervision.Service;
using Xunit;
namespace TR2.Supervision.Service.Tests;
public sealed class LiveOperatorTests
{
    private static ushort[] Guard() { var b = new ushort[29]; b[10]=4;b[14]=14;b[15]=13;b[16]=4;b[22]=0;b[23]=5;b[26]=0;b[27]=5;b[28]=1;return b; }
    [Fact] public void FreshGuardSkipsConsumedKeysAndLedgerIsMonotonic()
    {
        Assert.Equal((5u,(ushort)14),LiveOperatorConsoleApplication.Allocate(Guard(),null));
        Assert.Equal((5u,(ushort)21),LiveOperatorConsoleApplication.Allocate(Guard(),new(1,5,20)));
        var b=Guard();b[15]=30;Assert.Equal((5u,(ushort)31),LiveOperatorConsoleApplication.Allocate(b,new(1,5,20)));
    }
    [Theory] [InlineData(13,2)] [InlineData(10,9)] [InlineData(28,5)] [InlineData(23,0)]
    public void BadOrActiveGuardRejectsBeforeWrite(int offset,ushort word)
    { var b=Guard();b[offset]=word;Assert.Throws<InvalidDataException>(()=>LiveOperatorConsoleApplication.Allocate(b,null)); }
    [Fact] public void ExhaustionRollbackWrongIdentityAndMalformedGuardNeverRenewOrReuse()
    {
        Assert.Throws<InvalidDataException>(()=>LiveOperatorConsoleApplication.Allocate(Guard(),new(1,5,65534)));
        Assert.Throws<InvalidDataException>(()=>LiveOperatorConsoleApplication.Allocate(Guard(),new(1,6,14)));
        Assert.Throws<InvalidDataException>(()=>LiveOperatorConsoleApplication.Allocate(Guard(),new(2,5,14)));
        Assert.Throws<InvalidDataException>(()=>LiveOperatorConsoleApplication.Allocate([],null));
    }
    [Fact] public void TerminalRequiresExactEpochTransactionCodeAndNoActiveCommand()
    {
        var b=Guard();Assert.True(LiveOperatorConsoleApplication.IsTerminal(b,5,13,14));
        Assert.False(LiveOperatorConsoleApplication.IsTerminal(b,5,14,14));Assert.False(LiveOperatorConsoleApplication.IsTerminal(b,4,13,14));Assert.False(LiveOperatorConsoleApplication.IsTerminal(b,5,13,13));
        b[13]=2;Assert.False(LiveOperatorConsoleApplication.IsTerminal(b,5,13,14));b[13]=0;b[17]=1;Assert.False(LiveOperatorConsoleApplication.IsTerminal(b,5,13,14));
    }
    [Fact] public async Task InvalidCliCannotTouchSerialOrFiles()
    { using var output=new StringWriter();using var error=new StringWriter();Assert.Equal(2,await LiveOperatorConsoleApplication.RunAsync(["--live-command","start"],default,output,error)); }
}
