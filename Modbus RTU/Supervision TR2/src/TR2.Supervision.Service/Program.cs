namespace TR2.Supervision.Service;

internal static class Program
{
    private const ushort SupportedProtocolVersion = 1;

    private static async Task<int> Main(string[] args)
    {
        using var shutdown = new CancellationTokenSource();
        ConsoleCancelEventHandler cancelHandler = (_, eventArgs) =>
        {
            eventArgs.Cancel = true;
            shutdown.Cancel();
        };

        Console.CancelKeyPress += cancelHandler;
        try
        {
            return await SupervisionConsoleApplication.RunAsync(
                args,
                SupportedProtocolVersion,
                shutdown.Token,
                Console.Out,
                Console.Error);
        }
        finally
        {
            Console.CancelKeyPress -= cancelHandler;
        }
    }
}
