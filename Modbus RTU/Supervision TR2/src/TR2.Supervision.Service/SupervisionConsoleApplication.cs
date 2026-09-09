using TR2.Application;
using TR2.Supervision.Web;
using TR2.Transport;

namespace TR2.Supervision.Service;

public static class SupervisionConsoleApplication
{
    public const int SuccessExitCode = 0;
    public const int RuntimeFailureExitCode = 1;
    public const int UsageOrConfigurationExitCode = 2;

    private const int LatestCommunicationFailureDiagnosticLimit = 10;

    public static async Task<int> RunAsync(
        IReadOnlyList<string> args,
        ushort supportedProtocolVersion,
        CancellationToken cancellationToken,
        TextWriter output,
        TextWriter error,
        IModbusBusConnectionFactory? busConnectionFactory = null)
    {
        ArgumentNullException.ThrowIfNull(args);
        ArgumentNullException.ThrowIfNull(output);
        ArgumentNullException.ThrowIfNull(error);

        if (!TryParseConfigurationPath(args, out var configurationPath))
        {
            await error.WriteLineAsync("Usage: TR2.Supervision.Service --config <path>");
            return UsageOrConfigurationExitCode;
        }

        RuntimeConfiguration configuration;
        try
        {
            configuration = RuntimeConfigurationLoader.Load(configurationPath);
        }
        catch (Exception exception) when (
            exception is ArgumentException
            or InvalidDataException
            or IOException
            or UnauthorizedAccessException)
        {
            await error.WriteLineAsync($"Configuration error: {exception.Message}");
            return UsageOrConfigurationExitCode;
        }

        await WriteConfigurationSummaryAsync(configuration, configurationPath, output);

        try
        {
            var runtime = PhysicalSupervisionRuntimeFactory.Create(
                configuration,
                supportedProtocolVersion,
                busConnectionFactory);

            await output.WriteLineAsync("TR2 supervision starting.");
            await RunRuntimeWithOptionalWebAsync(runtime, configuration, cancellationToken, output);
            await output.WriteLineAsync("TR2 supervision stopped.");
            return SuccessExitCode;
        }
        catch (OperationCanceledException) when (cancellationToken.IsCancellationRequested)
        {
            await output.WriteLineAsync("TR2 supervision stopped.");
            return SuccessExitCode;
        }
        catch (Exception exception)
        {
            await error.WriteLineAsync($"Runtime failure: {exception.Message}");
            return RuntimeFailureExitCode;
        }
    }

    private static async Task RunRuntimeWithOptionalWebAsync(
        PhysicalSupervisionRuntime runtime,
        RuntimeConfiguration configuration,
        CancellationToken cancellationToken,
        TextWriter output)
    {
        using var coordinatedShutdown = CancellationTokenSource.CreateLinkedTokenSource(cancellationToken);
        var runtimeTask = runtime.Host.RunAsync(coordinatedShutdown.Token);
        await WriteRunningDiagnosticWhenAvailableAsync(runtime, runtimeTask, output);

        if (!configuration.Web.Enabled)
        {
            await runtimeTask;
            return;
        }

        var projection = new SupervisionReadProjection(
            runtime.Composition.FleetRegistry,
            runtime.Composition.TelemetrySnapshotRegistry,
            new SnapshotFreshnessPolicy(
                configuration.Web.FreshnessAgingAfter,
                configuration.Web.FreshnessStaleAfter));
        var webHost = new SupervisionWebHost(
            new SupervisionWebOptions(configuration.Web.ListenUri, configuration.Web.AllowRemote),
            projection,
            systemReadSource: new RuntimeSystemReadSource(runtime),
            commandSink: new RuntimeCommandSink(runtime));

        await output.WriteLineAsync(
            $"Web: enabled; listen={configuration.Web.ListenUri}; " +
            $"allowRemote={configuration.Web.AllowRemote.ToString().ToLowerInvariant()}");

        var webTask = webHost.RunAsync(coordinatedShutdown.Token);
        var completed = await Task.WhenAny(runtimeTask, webTask);

        if (cancellationToken.IsCancellationRequested)
        {
            coordinatedShutdown.Cancel();
            await AwaitShutdownAsync(runtimeTask);
            await AwaitShutdownAsync(webTask);
            return;
        }

        coordinatedShutdown.Cancel();
        var sibling = ReferenceEquals(completed, runtimeTask) ? webTask : runtimeTask;
        await AwaitShutdownAsync(sibling);

        if (completed.IsFaulted)
        {
            await completed;
        }

        throw new InvalidOperationException("A supervision component stopped unexpectedly.");
    }

    private static async Task AwaitShutdownAsync(Task task)
    {
        try
        {
            await task;
        }
        catch (OperationCanceledException)
        {
        }
    }

    private static async Task WriteRunningDiagnosticWhenAvailableAsync(
        PhysicalSupervisionRuntime runtime,
        Task runTask,
        TextWriter output)
    {
        while (!runTask.IsCompleted && runtime.Host.State != SupervisionRuntimeState.Running)
        {
            await Task.Delay(10);
        }

        if (runtime.Host.State != SupervisionRuntimeState.Running)
        {
            return;
        }

        var snapshot = RuntimeDiagnosticSnapshot.Capture(runtime);
        var connectedBuses = snapshot.ConnectedBusIds.Count == 0
            ? "none"
            : string.Join(",", snapshot.ConnectedBusIds);

        await output.WriteLineAsync(
            $"Runtime: state={snapshot.HostState}; ready={snapshot.IsReady.ToString().ToLowerInvariant()}; " +
            $"connectedBuses={connectedBuses}");

        foreach (var endpoint in snapshot.Endpoints)
        {
            var deviceId = endpoint.DeviceId?.ToString() ?? "none";
            await output.WriteLineAsync(
                $"Endpoint {endpoint.BusId}/{endpoint.Address}: " +
                $"session={endpoint.SessionState}; deviceId={deviceId}");
        }

        await WriteCommunicationFailureDiagnosticsAsync(runtime, output);
    }

    private static async Task WriteCommunicationFailureDiagnosticsAsync(
        PhysicalSupervisionRuntime runtime,
        TextWriter output)
    {
        var failures = runtime.Composition.CommunicationJournalSink
            .ReadLatest(LatestCommunicationFailureDiagnosticLimit);

        if (failures.Count == 0)
        {
            await output.WriteLineAsync("Communication failures: none");
            return;
        }

        await output.WriteLineAsync(
            $"Communication failures: latest={failures.Count}; limit={LatestCommunicationFailureDiagnosticLimit}");

        foreach (var failure in failures)
        {
            var deviceId = failure.DeviceId?.ToString() ?? "none";
            await output.WriteLineAsync(
                $"Communication failure {failure.FailureId}: " +
                $"endpoint={failure.BusId}/{failure.ModbusAddress}; deviceId={deviceId}; " +
                $"operation={failure.Operation}; category={failure.Category}; " +
                $"observedUtc={failure.ObservedAt:O}; exception={failure.ExceptionType}; message={failure.Message}");
        }
    }

    private static async Task WriteConfigurationSummaryAsync(
        RuntimeConfiguration configuration,
        string configurationPath,
        TextWriter output)
    {
        await output.WriteLineAsync($"Configuration: {Path.GetFullPath(configurationPath)}");
        await output.WriteLineAsync($"Database: {configuration.Persistence.DatabasePath}");
        await output.WriteLineAsync($"Buses: {configuration.Buses.Count}");
        await output.WriteLineAsync(
            $"Web configured: enabled={configuration.Web.Enabled.ToString().ToLowerInvariant()}; " +
            $"listen={configuration.Web.ListenUri}; " +
            $"allowRemote={configuration.Web.AllowRemote.ToString().ToLowerInvariant()}; " +
            $"freshnessMs={configuration.Web.FreshnessAgingAfter.TotalMilliseconds:0}/" +
            $"{configuration.Web.FreshnessStaleAfter.TotalMilliseconds:0}");

        foreach (var bus in configuration.Buses)
        {
            var endpoints = bus.Endpoints.Count == 0
                ? "none"
                : string.Join(",", bus.Endpoints.Select(endpoint => endpoint.Address.Value));

            if (bus.Serial is null)
            {
                await output.WriteLineAsync(
                    $"Bus {bus.Bus.Id}: logical-only; endpoints={endpoints}");
                continue;
            }

            await output.WriteLineAsync(
                $"Bus {bus.Bus.Id}: port={bus.Serial.PortName}; " +
                $"serial={bus.Serial.BaudRate}/{bus.Serial.DataBits}/{bus.Serial.Parity}/{bus.Serial.StopBits}; " +
                $"timeoutMs={bus.Serial.ResponseTimeout.TotalMilliseconds:0}; endpoints={endpoints}");
        }
    }

    private static bool TryParseConfigurationPath(
        IReadOnlyList<string> args,
        out string configurationPath)
    {
        configurationPath = string.Empty;

        if (args.Count != 2 || !string.Equals(args[0], "--config", StringComparison.Ordinal))
        {
            return false;
        }

        if (string.IsNullOrWhiteSpace(args[1]))
        {
            return false;
        }

        configurationPath = args[1];
        return true;
    }
}
