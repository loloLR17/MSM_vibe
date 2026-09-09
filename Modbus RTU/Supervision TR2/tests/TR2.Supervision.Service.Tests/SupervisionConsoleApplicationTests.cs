using TR2.Application;
using TR2.Domain;
using TR2.Persistence.Sqlite;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class SupervisionConsoleApplicationTests
{
    [Fact]
    public async Task MissingConfigArgumentReturnsUsageExitCodeWithoutStartingRuntime()
    {
        using var output = new StringWriter();
        using var error = new StringWriter();

        var exitCode = await SupervisionConsoleApplication.RunAsync(
            [],
            supportedProtocolVersion: 1,
            CancellationToken.None,
            output,
            error);

        Assert.Equal(SupervisionConsoleApplication.UsageOrConfigurationExitCode, exitCode);
        Assert.Contains("--config <path>", error.ToString(), StringComparison.Ordinal);
        Assert.Equal(string.Empty, output.ToString());
    }

    [Fact]
    public async Task InvalidConfigurationReturnsConfigurationExitCode()
    {
        var path = NewConfigPath();
        await File.WriteAllTextAsync(path, "{ invalid json }");
        try
        {
            using var output = new StringWriter();
            using var error = new StringWriter();

            var exitCode = await SupervisionConsoleApplication.RunAsync(
                ["--config", path],
                supportedProtocolVersion: 1,
                CancellationToken.None,
                output,
                error);

            Assert.Equal(SupervisionConsoleApplication.UsageOrConfigurationExitCode, exitCode);
            Assert.Contains("Configuration error:", error.ToString(), StringComparison.Ordinal);
            Assert.Equal(string.Empty, output.ToString());
        }
        finally
        {
            File.Delete(path);
        }
    }

    [Fact]
    public async Task RequestedCancellationStopsCleanlyWithSuccessExitCode()
    {
        var databasePath = NewDatabasePath();
        var configPath = NewConfigPath();
        await File.WriteAllTextAsync(
            configPath,
            $$"""
            {
              "persistence": { "databasePath": "{{databasePath.Replace("\\", "\\\\")}}" },
              "buses": []
            }
            """);

        try
        {
            using var output = new StringWriter();
            using var error = new StringWriter();
            using var cancellation = new CancellationTokenSource();
            cancellation.Cancel();

            var exitCode = await SupervisionConsoleApplication.RunAsync(
                ["--config", configPath],
                supportedProtocolVersion: 1,
                cancellation.Token,
                output,
                error);

            Assert.Equal(SupervisionConsoleApplication.SuccessExitCode, exitCode);
            Assert.Contains($"Configuration: {Path.GetFullPath(configPath)}", output.ToString(), StringComparison.Ordinal);
            Assert.Contains($"Database: {databasePath}", output.ToString(), StringComparison.Ordinal);
            Assert.Contains("Buses: 0", output.ToString(), StringComparison.Ordinal);
            Assert.Contains("TR2 supervision starting.", output.ToString(), StringComparison.Ordinal);
            Assert.Contains("TR2 supervision stopped.", output.ToString(), StringComparison.Ordinal);
            Assert.Equal(string.Empty, error.ToString());
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
            File.Delete(configPath);
        }
    }

    [Fact]
    public async Task RunningRuntimeDiagnosticReportsAuthoritativeState()
    {
        var databasePath = NewDatabasePath();
        var configPath = NewConfigPath();
        await File.WriteAllTextAsync(
            configPath,
            $$"""
            {
              "persistence": { "databasePath": "{{databasePath.Replace("\\", "\\\\")}}" },
              "buses": []
            }
            """);

        try
        {
            using var cancellation = new CancellationTokenSource();
            using var output = new CancellingStringWriter(cancellation, "Communication failures: none");
            using var error = new StringWriter();

            var exitCode = await SupervisionConsoleApplication.RunAsync(
                ["--config", configPath],
                supportedProtocolVersion: 1,
                cancellation.Token,
                output,
                error);

            Assert.Equal(SupervisionConsoleApplication.SuccessExitCode, exitCode);
            Assert.Contains(
                "Runtime: state=Running; ready=true; connectedBuses=none",
                output.ToString(),
                StringComparison.Ordinal);
            Assert.Contains("Communication failures: none", output.ToString(), StringComparison.Ordinal);
            Assert.Contains("TR2 supervision stopped.", output.ToString(), StringComparison.Ordinal);
            Assert.Equal(string.Empty, error.ToString());
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
            File.Delete(configPath);
        }
    }

    [Fact]
    public async Task RunningRuntimeDiagnosticReportsPersistedCommunicationFailure()
    {
        var databasePath = NewDatabasePath();
        var configPath = NewConfigPath();
        await File.WriteAllTextAsync(
            configPath,
            $$"""
            {
              "persistence": { "databasePath": "{{databasePath.Replace("\\", "\\\\")}}" },
              "buses": []
            }
            """);

        try
        {
            var database = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
            var sink = new SqliteCommunicationJournalSink(database);
            var observedAt = new DateTimeOffset(2026, 9, 9, 18, 30, 0, TimeSpan.Zero);
            await sink.AppendAsync(new CommunicationFailureEvent(
                new TR2Endpoint(new SerialBus("bus-a"), new ModbusAddress(5)),
                new DeviceId(42),
                CommunicationOperation.CommandTransaction,
                CommunicationFailureCategory.Io,
                observedAt,
                "System.IO.IOException",
                "simulated cable loss"));

            using var cancellation = new CancellationTokenSource();
            using var output = new CancellingStringWriter(cancellation, "message=simulated cable loss");
            using var error = new StringWriter();

            var exitCode = await SupervisionConsoleApplication.RunAsync(
                ["--config", configPath],
                supportedProtocolVersion: 1,
                cancellation.Token,
                output,
                error);

            Assert.Equal(SupervisionConsoleApplication.SuccessExitCode, exitCode);
            Assert.Contains("Communication failures: latest=1; limit=10", output.ToString(), StringComparison.Ordinal);
            Assert.Contains(
                "endpoint=bus-a/5; deviceId=42; operation=CommandTransaction; category=Io",
                output.ToString(),
                StringComparison.Ordinal);
            Assert.Contains("observedUtc=2026-09-09T18:30:00.0000000+00:00", output.ToString(), StringComparison.Ordinal);
            Assert.Contains("exception=System.IO.IOException; message=simulated cable loss", output.ToString(), StringComparison.Ordinal);
            Assert.Equal(string.Empty, error.ToString());
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
            File.Delete(configPath);
        }
    }

    [Fact]
    public async Task SerialStartupFailureReturnsRuntimeFailureExitCode()
    {
        var databasePath = NewDatabasePath();
        var configPath = NewConfigPath();
        await File.WriteAllTextAsync(
            configPath,
            $$"""
            {
              "persistence": { "databasePath": "{{databasePath.Replace("\\", "\\\\")}}" },
              "buses": [
                {
                  "id": "bus-1",
                  "serial": {
                    "portName": "COM7",
                    "baudRate": 115200,
                    "dataBits": 8,
                    "parity": "None",
                    "stopBits": "One",
                    "responseTimeoutMilliseconds": 750
                  },
                  "endpoints": [1]
                }
              ]
            }
            """);

        try
        {
            using var output = new StringWriter();
            using var error = new StringWriter();

            var exitCode = await SupervisionConsoleApplication.RunAsync(
                ["--config", configPath],
                supportedProtocolVersion: 1,
                CancellationToken.None,
                output,
                error,
                new FailingConnectionFactory());

            Assert.Equal(SupervisionConsoleApplication.RuntimeFailureExitCode, exitCode);
            Assert.Contains("Buses: 1", output.ToString(), StringComparison.Ordinal);
            Assert.Contains(
                "Bus bus-1: port=COM7; serial=115200/8/None/One; timeoutMs=750; endpoints=1",
                output.ToString(),
                StringComparison.Ordinal);
            Assert.Contains("TR2 supervision starting.", output.ToString(), StringComparison.Ordinal);
            Assert.Contains("Runtime failure: port unavailable", error.ToString(), StringComparison.Ordinal);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
            File.Delete(configPath);
        }
    }

    private static string NewConfigPath() =>
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s5b-{Guid.NewGuid():N}.json");

    private static string NewDatabasePath() =>
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s5b-{Guid.NewGuid():N}.db");

    private static void DeleteDatabaseFiles(string databasePath)
    {
        Microsoft.Data.Sqlite.SqliteConnection.ClearAllPools();
        foreach (var suffix in new[] { string.Empty, "-wal", "-shm" })
        {
            var path = databasePath + suffix;
            if (File.Exists(path))
            {
                File.Delete(path);
            }
        }
    }

    private sealed class CancellingStringWriter : StringWriter
    {
        private readonly CancellationTokenSource _cancellation;
        private readonly string _trigger;

        public CancellingStringWriter(CancellationTokenSource cancellation, string trigger)
        {
            _cancellation = cancellation;
            _trigger = trigger;
        }

        public override async Task WriteLineAsync(string? value)
        {
            await base.WriteLineAsync(value);
            if (value?.Contains(_trigger, StringComparison.Ordinal) == true)
            {
                _cancellation.Cancel();
            }
        }
    }

    private sealed class FailingConnectionFactory : IModbusBusConnectionFactory
    {
        public ValueTask<IModbusBusConnection> OpenAsync(
            string busId,
            ModbusSerialConnectionSettings settings,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromException<IModbusBusConnection>(new IOException("port unavailable"));
    }
}
