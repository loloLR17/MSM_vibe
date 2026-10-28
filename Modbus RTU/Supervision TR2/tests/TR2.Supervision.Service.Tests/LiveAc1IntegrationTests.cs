using System.Text.Json;
using TR2.Application;
using TR2.Domain;
using TR2.Transport;
using Xunit;
namespace TR2.Supervision.Service.Tests;
public sealed class LiveAc1IntegrationTests
{
 private sealed class Transport : IRegisterTransport
 {
  public bool Bad;public List<ushort> Reads=[];
  public ValueTask<ushort[]> ReadRegistersAsync(string bus,byte unit,ushort start,ushort count,CancellationToken ct=default)
  {
   Reads.Add(start);var r=new ushort[count];
   if(start==0){r[1]=1;r[6]=2;r[7]=12;}
   else if(start==4000){r[0]=2;r[1]=3;}
   else if(start==6000)r[0]=2;
   else if(start==3000)r=LiveSupervisionTests.Words(1);
   else if(start==8000){r[0]=(ushort)(Bad?0:0x4143);r[1]=1;r[2]=23;r[3]=1;r[4]=2;r[5]=0xAC;r[7]=26667;r[9]=2000;r[11]=26667;r[15]=18;r[17]=4;r[19]=4;r[21]=1;r[23]=4096;r[35]=19;r[43]=48;r[51]=1;r[52]=7;}
   else throw new InvalidOperationException("Unexpected read");
   return ValueTask.FromResult(r);
  }
 }
 private sealed class Sink:ILiveObservationSink
 {public LiveObservation? Saved;public ValueTask AppendAsync(LiveObservation o,CancellationToken ct=default){Saved=o;return ValueTask.CompletedTask;}}
 [Fact] public async Task ExperimentalAcIsSeparatelyArchivedAndB3SemanticsStayUnchanged()
 {
  var t=new Transport();var sink=new Sink();var service=new LiveObservationService(t,sink,allowDiagnostic:true,ac1Enabled:true);
  var result=await service.PollAsync(new(new SerialBus("test"),new ModbusAddress(1)));
  Assert.True(result.IsCurrent);Assert.Equal((uint)958,result.Observation.Snapshot!.Value.RmsGlobalMg);
  Assert.Equal((uint)19,result.Observation.DynamicAcceleration!.RmsGlobalMg);Assert.Null(result.Observation.RunId);Assert.Equal((ulong)18,result.Observation.DynamicAcceleration.RunId);
  var roundTrip=JsonSerializer.Deserialize<LiveObservation>(JsonSerializer.Serialize(sink.Saved));Assert.Equal((uint)19,roundTrip!.DynamicAcceleration!.RmsGlobalMg);
  Assert.Contains((ushort)8000,t.Reads);
 }
 [Fact] public async Task DisabledExtensionDoesNotProbeFrozenProfileAndInvalidAcNeverBecomesCurrent()
 {
  var t=new Transport();var sink=new Sink();var ep=new TR2Endpoint(new SerialBus("test"),new ModbusAddress(1));
  var old=await new LiveObservationService(t,sink,allowDiagnostic:true).PollAsync(ep);
  Assert.Null(old.Observation.DynamicAcceleration);Assert.DoesNotContain((ushort)8000,t.Reads);
  t.Bad=true;var failed=await new LiveObservationService(t,sink,allowDiagnostic:true,ac1Enabled:true).PollAsync(ep);
  Assert.False(failed.IsCurrent);Assert.Null(failed.Observation.DynamicAcceleration);Assert.Equal(LiveContinuity.Rejected,failed.Observation.Continuity);
 }
}
