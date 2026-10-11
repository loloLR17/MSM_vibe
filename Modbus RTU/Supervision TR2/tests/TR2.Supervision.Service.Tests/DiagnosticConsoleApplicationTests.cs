using System.Text.Json;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class DiagnosticConsoleApplicationTests
{
    [Theory]
    [InlineData(false)]
    [InlineData(true)]
    public async Task OfflineNeverOpensPortOrStartsProduction(bool configured)
    {
        var factory = new ProbeFactory();
        using var output = new StringWriter();
        using var error = new StringWriter();
        var args = configured ? SettingsArguments() : ["--diagnostic"];
        Assert.Equal(0, await Run(args, factory, output, error));
        Assert.Contains("OFFLINE", output.ToString());
        Assert.DoesNotContain("TR2 supervision starting", output.ToString());
        Assert.Equal(0, factory.OpenCount);
        Assert.Equal("", error.ToString());
    }

    [Theory]
    [InlineData("--config", "example.json")]
    [InlineData("--parity", "2")]
    [InlineData("--timeout-ms", "99")]
    [InlineData("--timeout-ms", "5001")]
    [InlineData("--baud", "0")]
    [InlineData("--read-once", "")]
    [InlineData("--unit", "2")]
    public async Task InvalidOrDuplicateOptionsNeverOpenPort(string option, string value)
    {
        var factory = new ProbeFactory();
        using var output = new StringWriter();
        using var error = new StringWriter();
        var args = SettingsArguments().Concat(value == "" ? [option] : new[] { option, value }).ToArray();
        Assert.Equal(2, await Run(args, factory, output, error));
        Assert.Equal(0, factory.OpenCount);
    }

    [Theory]
    [InlineData("--parity", "2")]
    [InlineData("--parity", "even")]
    [InlineData("--timeout-ms", "99")]
    [InlineData("--timeout-ms", "5001")]
    [InlineData("--baud", "0")]
    [InlineData("--port", "")]
    public async Task InvalidSettingsNeverOpenPort(string option, string value)
    {
        var factory = new ProbeFactory();
        using var output = new StringWriter();
        using var error = new StringWriter();
        var args = SettingsArguments();
        args[Array.IndexOf(args, option) + 1] = value;
        Assert.Equal(2, await Run(args, factory, output, error));
        Assert.Equal(0, factory.OpenCount);
    }

    [Theory]
    [InlineData("Even")]
    [InlineData("Odd")]
    public async Task OneExactReadLogsUnknownClosesAndNeverRequestsWriteTransport(string parity)
    {
        var path = NewLogPath();
        var factory = new ProbeFactory { LogPath = path };
        using var output = new StringWriter();
        using var error = new StringWriter();
        try
        {
            Assert.Equal(0, await Run(ReadArguments(path, parity), factory, output, error));
            Assert.Equal(1, factory.OpenCount);
            Assert.Equal("COM7", factory.Settings!.PortName);
            Assert.Equal(115200, factory.Settings.BaudRate);
            Assert.Equal(Enum.Parse<ModbusSerialParity>(parity), factory.Settings.Parity);
            Assert.Equal(8, factory.Settings.DataBits);
            Assert.Equal(ModbusSerialStopBits.One, factory.Settings.StopBits);
            Assert.Equal(1000, factory.Settings.ResponseTimeoutMilliseconds);
            Assert.Equal(1, factory.ReadCount);
            Assert.Equal(1, factory.DisposeCount);
            Assert.Equal("", error.ToString());
            Assert.Contains("UNKNOWN(0)", output.ToString());
            Assert.Contains("DeviceId unavailable", output.ToString());
            using var result = JsonDocument.Parse(File.ReadAllLines(path)[^1]);
            Assert.True(result.RootElement.GetProperty("passed").GetBoolean());
            Assert.Equal("result", result.RootElement.GetProperty("event").GetString());
        }
        finally { File.Delete(path); }
    }

    [Theory]
    [InlineData(0, 0)]
    [InlineData(2, 0)]
    [InlineData(1, 1)]
    public async Task IncorrectDecodedSizeOrValueFailsAndCloses(int count, ushort value)
    {
        await CheckFailure(new ProbeFactory { Registers = Enumerable.Repeat(value, count).ToArray() }, "read", 1);
    }

    [Theory]
    [InlineData("timeout")]
    [InlineData("crc")]
    [InlineData("exception")]
    [InlineData("busy")]
    public async Task CommunicationAndOpenErrorsFailWithoutRetry(string failure)
    {
        var factory = new ProbeFactory();
        if (failure == "busy") { factory.OpenException = new UnauthorizedAccessException("port occupied"); }
        else
        {
            factory.ReadException = failure switch
            {
                "timeout" => new ModbusTransportFailureException(ModbusTransportFailureKind.Timeout, "timeout", new TimeoutException()),
                "crc" => new ModbusTransportFailureException(ModbusTransportFailureKind.Io, "invalid CRC", new IOException()),
                _ => new ModbusTransportFailureException(ModbusTransportFailureKind.ModbusExceptionResponse,
                    "exception 04", new Exception(), 0x83, 4)
            };
        }
        await CheckFailure(factory, failure == "busy" ? "open" : "read", failure == "busy" ? 0 : 1);
    }

    [Fact]
    public async Task ExistingJournalIsPreservedAndPreventsOpening()
    {
        var path = NewLogPath();
        File.WriteAllText(path, "existing proof");
        try
        {
            var factory = new ProbeFactory();
            using var output = new StringWriter();
            using var error = new StringWriter();
            Assert.Equal(1, await Run(ReadArguments(path), factory, output, error));
            Assert.Equal(0, factory.OpenCount);
            Assert.Equal("existing proof", File.ReadAllText(path));
        }
        finally { File.Delete(path); }
    }

    [Fact]
    public async Task CancellationBeforeOpeningCreatesNoJournalOrTraffic()
    {
        var path = NewLogPath();
        var factory = new ProbeFactory();
        using var output = new StringWriter();
        using var error = new StringWriter();
        Assert.Equal(1, await SupervisionConsoleApplication.RunAsync(ReadArguments(path), 1,
            new CancellationToken(true), output, error, factory));
        Assert.Equal(0, factory.OpenCount);
        Assert.False(File.Exists(path));
    }

    [Fact]
    public async Task StalledReadIsBoundedAndConnectionDisposedWithoutAnotherRead()
    {
        var path = NewLogPath();
        var factory = new ProbeFactory { LogPath = path, Stalled = true };
        using var output = new StringWriter();
        using var error = new StringWriter();
        try
        {
            var args = ReadArguments(path);
            args[Array.IndexOf(args, "--timeout-ms") + 1] = "100";
            var run = Run(args, factory, output, error);
            Assert.Equal(1, await run.WaitAsync(TimeSpan.FromSeconds(5)));
            Assert.Equal(1, factory.ReadCount);
            Assert.Equal(1, factory.DisposeCount);
            Assert.Contains("TimeoutException", error.ToString());
        }
        finally { File.Delete(path); }
    }

    private static async Task CheckFailure(ProbeFactory factory, string stage, int reads)
    {
        var path = NewLogPath();
        factory.LogPath = path;
        using var output = new StringWriter();
        using var error = new StringWriter();
        try
        {
            Assert.Equal(1, await Run(ReadArguments(path), factory, output, error));
            Assert.Equal(1, factory.OpenCount);
            Assert.Equal(reads, factory.ReadCount);
            Assert.Equal(reads, factory.DisposeCount);
            using var result = JsonDocument.Parse(File.ReadAllLines(path)[^1]);
            Assert.False(result.RootElement.GetProperty("passed").GetBoolean());
            Assert.Equal(stage, result.RootElement.GetProperty("stage").GetString());
            Assert.DoesNotContain("DIAGNOSTIC response:", output.ToString());
        }
        finally { File.Delete(path); }
    }

    private static Task<int> Run(string[] args, ProbeFactory factory, TextWriter output, TextWriter error) =>
        SupervisionConsoleApplication.RunAsync(args, 1, CancellationToken.None, output, error, factory);

    private static string NewLogPath() => Path.Combine(Path.GetTempPath(), $"tr2-m014-{Guid.NewGuid():N}.jsonl");
    private static string[] SettingsArguments(string parity = "Even") =>
        ["--diagnostic", "--port", "COM7", "--baud", "115200", "--parity", parity, "--timeout-ms", "1000"];
    private static string[] ReadArguments(string path, string parity = "Even") =>
        [.. SettingsArguments(parity), "--read-once", "--log", path];

    private sealed class ProbeFactory : IModbusBusConnectionFactory, IModbusBusConnection, IRegisterTransport
    {
        public string BusId { get; private set; } = "";
        public IRegisterTransport RegisterTransport => this;
        public IRegisterWriteTransport RegisterWriteTransport => throw new InvalidOperationException("Write access forbidden");
        public ModbusSerialConnectionSettings? Settings { get; private set; }
        public string? LogPath { get; set; }
        public ushort[] Registers { get; init; } = [0];
        public Exception? ReadException { get; set; }
        public Exception? OpenException { get; set; }
        public bool Stalled { get; init; }
        public int OpenCount { get; private set; }
        public int ReadCount { get; private set; }
        public int DisposeCount { get; private set; }
        private readonly TaskCompletionSource<ushort[]> _pending = new(TaskCreationOptions.RunContinuationsAsynchronously);

        public ValueTask<IModbusBusConnection> OpenAsync(string busId, ModbusSerialConnectionSettings settings,
            CancellationToken cancellationToken = default)
        {
            Assert.True(File.Exists(LogPath));
            OpenCount++;
            BusId = busId;
            Settings = settings;
            return OpenException is null ? ValueTask.FromResult<IModbusBusConnection>(this) :
                ValueTask.FromException<IModbusBusConnection>(OpenException);
        }

        public ValueTask<ushort[]> ReadRegistersAsync(string busId, byte unitAddress, ushort startAddress,
            ushort registerCount, CancellationToken cancellationToken = default)
        {
            Assert.Equal(BusId, busId);
            Assert.Equal((byte)1, unitAddress);
            Assert.Equal((ushort)1000, startAddress);
            Assert.Equal((ushort)1, registerCount);
            ReadCount++;
            if (Stalled) { return new ValueTask<ushort[]>(_pending.Task); }
            return ReadException is null ? ValueTask.FromResult(Registers) : ValueTask.FromException<ushort[]>(ReadException);
        }

        public ValueTask DisposeAsync()
        {
            DisposeCount++;
            _pending.TrySetException(new IOException("simulated port closed"));
            return ValueTask.CompletedTask;
        }
    }
}
