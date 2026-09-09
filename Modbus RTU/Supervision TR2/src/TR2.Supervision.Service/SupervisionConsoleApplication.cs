using TR2.Transport;

namespace TR2.Supervision.Service;

public static class SupervisionConsoleApplication
{
    public const int SuccessExitCode = 0;
    public const int RuntimeFailureExitCode = 1;
    public const int UsageOrConfigurationExitCode = 2;

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
            var runTask = runtime.Host.RunAsync(cancellationToken);
            await WriteRunningDiagnosticWhenAvailableAsync(runtime, runTask, output);
            await runTask;
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
    }

    private static async Task WriteConfigurationSummaryAsync(
        RuntimeConfiguration configuration,
        string configurationPath,
        TextWriter output)
    {
        await output.WriteLineAsync($"Configuration: {Path.GetFullPath(configurationPath)}");
        await output.WriteLineAsync($"Database: {configuration.Persistence.DatabasePath}");
        await output.WriteLineAsync($"Buses: {configuration.Buses.Count}");

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
