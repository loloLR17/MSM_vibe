using TR2.Supervision.Service;
using Xunit;
namespace TR2.Supervision.Service.Tests;
public sealed class LiveOperatorConfigurationTests
{
 [Fact] public void MapsEntireFrozenLiveVectorWithIndependentCrcAndReservedZeros()
 {
  ushort[] active={26667,7,2,2,32768,5000,0,0,0,0,0,0,0,0,0,0,1,100,200,300,400,20,500,0,0,0,0,0,0,1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,1,0,0,0,0,0,0,0,0}; ushort[] expected={26667,0,7,2,2,32768,5000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,100,200,300,400,20,500,0,0,0,0,0,0,0,0,0,0,1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,1,0,0,0,0};
  var actual=LiveOperatorConfiguration.Prepare8G(active);
  Assert.Equal(expected,actual);Assert.Equal(2074629576u,LiveOperatorConfiguration.Crc(actual));
 }
 [Fact] public void RejectsLegacyCampaignAndMalformedInputsBeforeAnyStaging()
 {Assert.Throws<InvalidDataException>(()=>LiveOperatorConfiguration.Prepare8G(new ushort[75]));var a=new ushort[76];a[0]=26667;a[3]=1;Assert.Throws<InvalidDataException>(()=>LiveOperatorConfiguration.Prepare8G(a));}
}
