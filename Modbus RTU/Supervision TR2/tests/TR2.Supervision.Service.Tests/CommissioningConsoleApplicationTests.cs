using System.Text.Json;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class CommissioningConsoleApplicationTests
{
    private static ushort[] B0() { var values=new ushort[21];values[1]=1;values[6]=1;return values; }
    private static string[] Args(string b0,string log,bool execute=true) =>
        ["--commissioning-check","--protocol-version","1","--expected-b0",b0,
         "--port","COM7","--baud","115200","--parity","Even","--timeout-ms","100",
         ..(execute?new[]{"--log",log,"--execute"}:Array.Empty<string>())];
    private static async Task<int> Run(string[] args, Factory factory, List<double>? waits=null)
    {
        using var output=new StringWriter();using var error=new StringWriter();
        return await CommissioningConsoleApplication.RunAsync(args,default,output,error,factory,
            (duration,token)=>{waits?.Add(duration.TotalSeconds);return Task.CompletedTask;});
    }
    [Fact]
    public async Task OfflineCannotOpenOrCreateJournal()
    {
        var folder=NewFolder();var b0=Path.Combine(folder,"b0.json");var log=Path.Combine(folder,"events.jsonl");
        try{File.WriteAllText(b0,JsonSerializer.Serialize(B0()));var factory=new Factory();
            Assert.Equal(0,await Run(Args(b0,log,false),factory));Assert.Equal(0,factory.Opens);
            Assert.False(File.Exists(log));Assert.False(File.Exists(Path.ChangeExtension(log,".sqlite")));}
        finally{Directory.Delete(folder,true);}
    }
    [Fact]
    public async Task ExactlyNineReadRequestsPreserveInvalidateAndRefresh()
    {
        var folder=NewFolder();var b0=Path.Combine(folder,"b0.json");var log=Path.Combine(folder,"events.jsonl");
        try{File.WriteAllText(b0,JsonSerializer.Serialize(B0()));var factory=new Factory();var waits=new List<double>();
            Assert.Equal(0,await Run(Args(b0,log),factory,waits));Assert.Equal(2,factory.Opens);Assert.Equal(2,factory.Closes);
            Assert.Equal(9,factory.Reads.Count);Assert.Equal(new double[]{2,2,100,100,2,2},waits);
            Assert.Equal(new ushort[]{0,1000,1000,1000,1000,0,1000,1000,1000},factory.Reads.Select(r=>r.Address));
            Assert.All(factory.Reads,r=>Assert.Equal(r.Address==0?21:20,r.Count));
            using var result=JsonDocument.Parse(File.ReadAllLines(log)[^1]);
            var root=result.RootElement;Assert.True(root.GetProperty("passed").GetBoolean());
            Assert.False(root.GetProperty("afterLoss").GetProperty("IsAvailable").GetBoolean());
            Assert.Equal(root.GetProperty("initial").GetProperty("ReceivedAt").GetString(),root.GetProperty("afterLoss").GetProperty("ReceivedAt").GetString());
            Assert.True(root.GetProperty("recovered").GetProperty("IsAvailable").GetBoolean());
            Assert.NotEqual(root.GetProperty("initial").GetProperty("ReceivedAt").GetString(),root.GetProperty("recovered").GetProperty("ReceivedAt").GetString());
        }finally{Directory.Delete(folder,true);}
    }
    [Theory]
    [InlineData("wrong-B0",1)]
    [InlineData("short-B1",2)]
    [InlineData("early-timeout",1)]
    [InlineData("no-loss",5)]
    [InlineData("loss-io",5)]
    [InlineData("changed-B0",6)]
    [InlineData("close",5)]
    [InlineData("recovery-timeout",7)]
    public async Task AnomalyStopsWithoutRetry(string scenario,int requests)
    {
        var folder=NewFolder();var b0=Path.Combine(folder,"b0.json");var log=Path.Combine(folder,"events.jsonl");
        try{File.WriteAllText(b0,JsonSerializer.Serialize(B0()));var factory=new Factory{Scenario=scenario};
            Assert.Equal(1,await Run(Args(b0,log),factory));Assert.Equal(requests,factory.Reads.Count);
            Assert.Contains("\"passed\":false",File.ReadAllText(log));}
        finally{Directory.Delete(folder,true);}
    }
    [Theory]
    [InlineData("missing-version")]
    [InlineData("duplicate-execute")]
    [InlineData("bad-port")]
    [InlineData("bad-b0")]
    public async Task InvalidPlanNeverOpens(string scenario)
    {
        var folder=NewFolder();var b0=Path.Combine(folder,"b0.json");var log=Path.Combine(folder,"events.jsonl");
        try{File.WriteAllText(b0,JsonSerializer.Serialize(B0()));var args=Args(b0,log).ToList();
            if(scenario=="missing-version")args.RemoveRange(1,2);
            if(scenario=="duplicate-execute")args.Add("--execute");
            if(scenario=="bad-port")args[6]="COM3";
            if(scenario=="bad-b0")File.WriteAllText(b0,"[0]");
            var factory=new Factory();Assert.Equal(2,await Run(args.ToArray(),factory));Assert.Equal(0,factory.Opens);Assert.False(File.Exists(log));}
        finally{Directory.Delete(folder,true);}
    }
    [Fact]
    public async Task ExistingJournalIsPreserved()
    {
        var folder=NewFolder();var b0=Path.Combine(folder,"b0.json");var log=Path.Combine(folder,"events.jsonl");
        try{File.WriteAllText(b0,JsonSerializer.Serialize(B0()));File.WriteAllText(log,"preserve");var factory=new Factory();
            Assert.Equal(1,await Run(Args(b0,log),factory));Assert.Equal(0,factory.Opens);Assert.Equal("preserve",File.ReadAllText(log));}
        finally{Directory.Delete(folder,true);}
    }
    private static string NewFolder(){var path=Path.Combine(Path.GetTempPath(),"TR2-M014-G-"+Guid.NewGuid());Directory.CreateDirectory(path);return path;}
    private sealed class Factory:IModbusBusConnectionFactory
    {
        public int Opens,Closes;public string Scenario="";
        public List<(ushort Address,ushort Count)> Reads=[];
        public ValueTask<IModbusBusConnection> OpenAsync(string busId,ModbusSerialConnectionSettings settings,CancellationToken token=default)
        {Opens++;Assert.Equal("COM7",settings.PortName);Assert.Equal(115200,settings.BaudRate);Assert.Equal(ModbusSerialParity.Even,settings.Parity);return ValueTask.FromResult<IModbusBusConnection>(new Connection(this,busId));}
    }
    private sealed class Connection(Factory factory,string busId):IModbusBusConnection,IRegisterTransport
    {
        public string BusId=>busId;
        public IRegisterTransport RegisterTransport=>this;
        public IRegisterWriteTransport RegisterWriteTransport=>throw new InvalidOperationException("No writes allowed");
        public ValueTask DisposeAsync(){factory.Closes++;if(factory.Scenario=="close")throw new IOException("Close failed");return ValueTask.CompletedTask;}
        public ValueTask<ushort[]> ReadRegistersAsync(string bus,byte unit,ushort address,ushort count,CancellationToken token=default)
        {
            Assert.Equal(BusId,bus);Assert.Equal(1,unit);factory.Reads.Add((address,count));var n=factory.Reads.Count;
            if(factory.Scenario=="early-timeout"&&n==1||factory.Scenario=="recovery-timeout"&&n==7||n==5&&factory.Scenario!="no-loss")
                throw new ModbusTransportFailureException(factory.Scenario=="loss-io"?ModbusTransportFailureKind.Io:ModbusTransportFailureKind.Timeout,"Physical fixture loss",new TimeoutException());
            var data=address==0?B0():new ushort[20];
            if(factory.Scenario=="wrong-B0"&&n==1||factory.Scenario=="changed-B0"&&n==6)data[1]=2;
            if(address==1000)data[5]=(ushort)n;
            if(factory.Scenario=="short-B1"&&n==2)data=data[..^1];
            return ValueTask.FromResult(data);
        }
    }
}
