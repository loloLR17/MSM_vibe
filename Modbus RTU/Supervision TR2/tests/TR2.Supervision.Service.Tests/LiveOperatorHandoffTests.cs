using TR2.Transport;
using Xunit;
namespace TR2.Supervision.Service.Tests;
public sealed class LiveOperatorHandoffTests
{
    [Theory] [InlineData(false,false)] [InlineData(true,false)] [InlineData(false,true)] [InlineData(true,true)]
    public async Task TokenAcknowledgmentPrecedesOpenLedgerPrecedesWriteAndWriterClosesBeforeResume(bool writeFails,bool initiallyHeld)
    {
        var dir=Path.Combine(Path.GetTempPath(),"live-handoff-"+Guid.NewGuid());Directory.CreateDirectory(dir);
        var control=Path.Combine(dir,"pause"); var ledger=Path.Combine(dir,"transactions.json");
        var factory=new Factory(control,ledger,writeFails,initiallyHeld);
        var ack=Task.Run(async()=>{for(var i=0;i<100 && !File.Exists(control);i++)await Task.Delay(10);var token=LiveDirectConsoleApplication.ReadControlFile(control);Assert.StartsWith("disconnected:",token);File.WriteAllText(control+".released",token);});
        using var output=new StringWriter();using var error=new StringWriter();
        try
        {
            var result=await LiveOperatorConsoleApplication.RunAsync(["--live-command","start","--port","COM4","--control",control,"--state",ledger],default,output,error,factory);
            await ack;Assert.Equal(writeFails?1:0,result);Assert.True(factory.Closed);Assert.False(File.Exists(control));Assert.True(File.Exists(ledger));
            Assert.Equal(writeFails?1:2,factory.Writes);Assert.Equal(!writeFails,output.ToString().Contains("terminal_and_effect_observed",StringComparison.Ordinal));
        }
        finally{Directory.Delete(dir,true);}
    }
    private sealed class Factory(string control,string ledger,bool fail,bool initiallyHeld):IModbusBusConnectionFactory
    {
        public bool Closed;public int Writes;public bool InitiallyHeld=initiallyHeld;
        public ValueTask<IModbusBusConnection> OpenAsync(string bus,ModbusSerialConnectionSettings settings,CancellationToken ct=default)
        {Assert.Equal(LiveDirectConsoleApplication.ReadControlFile(control),LiveDirectConsoleApplication.ReadControlFile(control+".released"));return ValueTask.FromResult<IModbusBusConnection>(new Connection(this,control,ledger,fail));}
        private sealed class Connection(Factory owner,string control,string ledger,bool fail):IModbusBusConnection,IRegisterTransport,IRegisterWriteTransport
        {
            public string BusId=>"operator";public IRegisterTransport RegisterTransport=>this;public IRegisterWriteTransport RegisterWriteTransport=>this;
            public ValueTask<ushort[]> ReadRegistersAsync(string bus,byte unit,ushort address,ushort count,CancellationToken ct=default)
            {
                var b=new ushort[count];
                if(address==0){b[1]=1;b[6]=2;b[7]=12;}
                if(address==3000){b[0]=(ushort)(owner.Writes==2?4:1);b[1]=(ushort)(owner.Writes==2?6:owner.InitiallyHeld?5819:4097);}
                if(address==5000){b[10]=4;b[14]=(ushort)(owner.Writes==2?13:14);b[15]=(ushort)(owner.Writes==2?14:13);b[16]=4;b[23]=5;b[27]=5;b[28]=1;}
                return ValueTask.FromResult(b);
            }
            public ValueTask WriteRegistersAsync(string bus,byte unit,ushort address,IReadOnlyList<ushort> values,CancellationToken ct=default)
            {
                Assert.True(File.Exists(control));Assert.Contains("14",File.ReadAllText(ledger));owner.Writes++;
                if(owner.Writes==1){Assert.Equal((ushort)5020,address);Assert.Equal(new ushort[]{0,5},values);}
                else {Assert.Equal((ushort)5000,address);Assert.Equal(new ushort[]{13,14,0,0,0,0,0,1},values);}
                if(fail)throw new IOException("Simulated write uncertainty; no retry");return ValueTask.CompletedTask;
            }
            public ValueTask DisposeAsync(){Assert.True(File.Exists(control));owner.Closed=true;return ValueTask.CompletedTask;}
        }
    }
}
