using TR2.Protocol;
using Xunit;
namespace TR2.Protocol.Tests;
public sealed class LiveAc1SnapshotTests
{
    private static ushort[] Frame()
    {
        var r=new ushort[64];r[0]=0x4143;r[1]=1;r[2]=23;r[3]=1;r[4]=2;r[5]=0xAC;
        r[7]=26667;r[9]=2000;r[11]=26667;r[15]=18;r[17]=4;r[19]=4;r[21]=1;r[23]=4096;
        r[25]=154;r[27]=100;r[28]=0xFFF1;r[29]=0x8110; // -950000 micro-g, signed
        r[35]=19;r[37]=11;r[39]=10;r[41]=12;r[43]=48;r[45]=30;r[47]=35;r[49]=40;r[51]=12;r[52]=7;
        return r;
    }
    [Fact] public void ParsesIndependentDynamicUnitsAndSignedDcWithPhysicalFsEvidence()
    {
        var s=LiveAc1Snapshot.Parse(Frame());Assert.Equal(-950000,s.DcXMicroG);Assert.Equal((uint)19,s.RmsGlobalMg);
        Assert.Equal((ulong)18,s.RunId);Assert.True(s.FullScaleProved&&s.ValuesValid&&s.Fresh&&s.Running);Assert.False(s.Held);
        Assert.Contains("experimental",s.Extension);Assert.Equal("mg",s.Unit);
    }
    [Theory] [InlineData(0,0)] [InlineData(1,2)] [InlineData(3,2)] [InlineData(5,0xA0)] [InlineData(53,1)]
    public void RejectsVersionMethodReservedAndContradictoryFs(int at,int value)
    {var r=Frame();r[at]=(ushort)value;Assert.Throws<InvalidDataException>(()=>LiveAc1Snapshot.Parse(r));}
    [Fact] public void HeldAndWarmupAreDistinctAndCannotBeFresh()
    {var r=Frame();r[2]=25;var s=LiveAc1Snapshot.Parse(r);Assert.True(s.Held&&!s.Fresh&&!s.Running);r[2]=55;Assert.Throws<InvalidDataException>(()=>LiveAc1Snapshot.Parse(r));}
}
