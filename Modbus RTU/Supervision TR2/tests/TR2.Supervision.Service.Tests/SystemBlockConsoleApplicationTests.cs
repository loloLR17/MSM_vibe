using System.Text.Json;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class SystemBlockConsoleApplicationTests
{
    [Fact]
    public async Task OfflineDispatchDoesNotOpenOrComposeRuntime()
    {
        var factory = new Factory();
        using var output = new StringWriter();
        using var error = new StringWriter();
        Assert.Equal(0, await SupervisionConsoleApplication.RunAsync(
            ["--system-check", "--protocol-version", "1"], 1, default, output, error, factory));
        Assert.Equal(0, factory.Opens);
        Assert.Contains("OFFLINE", output.ToString());
        Assert.DoesNotContain("TR2 supervision starting", output.ToString());
    }

    [Theory]
    [InlineData("missing-version")]
    [InlineData("duplicate-version")]
    [InlineData("bad-version")]
    [InlineData("no-log")]
    [InlineData("config")]
    public async Task InvalidActivationCannotOpen(string scenario)
    {
        var args = Args("unused.jsonl").ToList();
        if (scenario == "missing-version") args.RemoveRange(1, 2);
        if (scenario == "duplicate-version") args.AddRange(["--protocol-version", "1"]);
        if (scenario == "bad-version") args[2] = "65536";
        if (scenario == "no-log") args.RemoveRange(args.Count - 2, 2);
        if (scenario == "config") args.AddRange(["--config", "production.json"]);
        var factory = new Factory();
        Assert.Equal(2, await Run(args, factory));
        Assert.Equal(0, factory.Opens);
    }

    [Fact]
    public async Task CompatibleB0ThenFullB1DecodedWithoutInventingReady()
    {
        var path = NewPath();
        var factory = new Factory();
        try
        {
            Assert.Equal(0, await Run(Args(path), factory));
            Assert.Equal(new[] { ((ushort)0, (ushort)21), ((ushort)1000, (ushort)20) }, factory.Reads);
            Assert.Equal(1, factory.Closes);
            Assert.Equal(ModbusSerialParity.Even, factory.Settings!.Parity);
            using var result = JsonDocument.Parse(File.ReadAllLines(path)[^1]);
            Assert.True(result.RootElement.GetProperty("passed").GetBoolean());
            Assert.Equal(0x12345678u, result.RootElement.GetProperty("deviceId").GetUInt32());
            Assert.Equal(0, result.RootElement.GetProperty("systemState").GetProperty("SystemStatus").GetInt32());
            Assert.Equal(-123, result.RootElement.GetProperty("systemState").GetProperty("InternalTemperatureDeciCelsius").GetInt32());
        }
        finally { File.Delete(path); }
    }

    [Theory]
    [InlineData("incompatible", 1)]
    [InlineData("short-B0", 1)]
    [InlineData("long-B0", 1)]
    [InlineData("exception-B0", 1)]
    [InlineData("timeout-B0", 1)]
    [InlineData("short-B1", 2)]
    [InlineData("io-B1", 2)]
    [InlineData("close", 2)]
    public async Task FailureStopsWithoutRetryAndCloses(string scenario, int reads)
    {
        var path = NewPath();
        var factory = new Factory { Scenario = scenario };
        try
        {
            Assert.Equal(1, await Run(Args(path), factory));
            Assert.Equal(reads, factory.Reads.Count);
            Assert.Equal(1, factory.Opens);
            Assert.Equal(1, factory.Closes);
            using var failure = JsonDocument.Parse(File.ReadAllLines(path)[^1]);
            Assert.Equal("failure", failure.RootElement.GetProperty("event").GetString());
            Assert.False(failure.RootElement.GetProperty("passed").GetBoolean());
        }
        finally { File.Delete(path); }
    }

    [Fact]
    public async Task ExistingJournalAndPreCancelledRunCannotOpenOrOverwrite()
    {
        var path = NewPath();
        try
        {
            File.WriteAllText(path, "preserve");
            var factory = new Factory();
            Assert.Equal(1, await Run(Args(path), factory));
            Assert.Equal(0, factory.Opens);
            Assert.Equal("preserve", File.ReadAllText(path));
            using var cancelled = new CancellationTokenSource();
            cancelled.Cancel();
            Assert.Equal(1, await Run(Args(path), factory, cancelled.Token));
            Assert.Equal(0, factory.Opens);
            Assert.Equal("preserve", File.ReadAllText(path));
        }
        finally { File.Delete(path); }
    }

    private static string NewPath() => Path.Combine(Path.GetTempPath(), $"tr2-system-check-{Guid.NewGuid():N}.jsonl");
    private static string[] Args(string path) => ["--system-check", "--protocol-version", "1", "--port", "COM7", "--baud", "115200", "--parity", "Even", "--timeout-ms", "100", "--read-once", "--log", path];
    private static async Task<int> Run(IReadOnlyList<string> args, Factory factory, CancellationToken token = default)
    {
        using var output = new StringWriter();
        using var error = new StringWriter();
        return await SystemBlockConsoleApplication.RunAsync(args, token, output, error, factory);
    }

    private sealed class Factory : IModbusBusConnectionFactory, IModbusBusConnection, IRegisterTransport
    {
        public string Scenario { get; init; } = "nominal";
        public int Opens { get; private set; }
        public int Closes { get; private set; }
        public ModbusSerialConnectionSettings? Settings { get; private set; }
        public List<(ushort Start, ushort Count)> Reads { get; } = [];
        public string BusId { get; private set; } = "";
        public IRegisterTransport RegisterTransport => this;
        public IRegisterWriteTransport RegisterWriteTransport => throw new InvalidOperationException("Writes prohibited");
        public ValueTask<IModbusBusConnection> OpenAsync(string busId, ModbusSerialConnectionSettings settings, CancellationToken cancellationToken = default)
        {
            Opens++; BusId = busId; Settings = settings;
            return ValueTask.FromResult<IModbusBusConnection>(this);
        }
        public ValueTask DisposeAsync()
        {
            Closes++;
            if (Scenario == "close") throw new IOException("Close failed");
            return ValueTask.CompletedTask;
        }
        public ValueTask<ushort[]> ReadRegistersAsync(string busId, byte unitAddress, ushort startAddress, ushort registerCount, CancellationToken cancellationToken = default)
        {
            Assert.Equal(BusId, busId);
            Assert.Equal(1, unitAddress);
            Reads.Add((startAddress, registerCount));
            if (Scenario == "timeout-B0") return new(new TaskCompletionSource<ushort[]>().Task);
            if (Scenario == "exception-B0") throw new ModbusTransportFailureException(ModbusTransportFailureKind.ModbusExceptionResponse, "Unavailable", new IOException());
            if (Scenario == "io-B1" && startAddress == 1000) throw new ModbusTransportFailureException(ModbusTransportFailureKind.Io, "Removed", new IOException());
            var length = registerCount;
            if (Scenario == "short-B0" && startAddress == 0 || Scenario == "short-B1" && startAddress == 1000) length--;
            if (Scenario == "long-B0" && startAddress == 0) length++;
            var registers = new ushort[length];
            if (startAddress == 0) { registers[0] = 0x1234; registers[1] = 0x5678; registers[6] = (ushort)(Scenario == "incompatible" ? 2 : 1); }
            else { registers[7] = unchecked((ushort)-123); }
            return ValueTask.FromResult(registers);
        }
    }
}
