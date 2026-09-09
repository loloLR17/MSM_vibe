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

        try
        {
            var runtime = PhysicalSupervisionRuntimeFactory.Create(
                configuration,
                supportedProtocolVersion,
                busConnectionFactory);

            await output.WriteLineAsync("TR2 supervision starting.");
            await runtime.Host.RunAsync(cancellationToken);
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
