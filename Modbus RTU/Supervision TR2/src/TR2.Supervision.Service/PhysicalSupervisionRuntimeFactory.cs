namespace TR2.Supervision.Service;

public sealed record PhysicalSupervisionRuntime(
    SupervisionRuntimeComposition Composition,
    SupervisionOperationalFacade Operations,
    SupervisionRuntimeHost Host);

public static class PhysicalSupervisionRuntimeFactory
{
    public static PhysicalSupervisionRuntime Create(
        RuntimeConfiguration configuration,
        ushort supportedProtocolVersion,
        TR2.Transport.IModbusBusConnectionFactory? busConnectionFactory = null,
        TimeProvider? timeProvider = null)
    {
        ArgumentNullException.ThrowIfNull(configuration);

        var composition = SupervisionRuntimeCompositionRoot.Compose(
            configuration,
            busConnectionFactory);
        var operations = new SupervisionOperationalFacade(composition);
        var priorityRunner = new PhysicalB5LifecycleWorkRunner(
            composition,
            operations);
        var pollingRunner = new PhysicalPollingWorkRunner(
            composition,
            supportedProtocolVersion,
            priorityRunner.ObserveCompatibleSession);
        var pollingLoop = new SupervisionPollingLoop(
            composition,
            pollingRunner,
            priorityRunner,
            timeProvider);
        var host = new SupervisionRuntimeHost(
            composition,
            pollingLoop);

        return new PhysicalSupervisionRuntime(
            composition,
            operations,
            host);
    }
}
