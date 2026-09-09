using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class PhysicalSupervisionRuntimeFactoryTests
{
    [Fact]
    public void Create_ComposesPhysicalHostWithoutOpeningHardware()
    {
        var databasePath = Path.Combine(
            Path.GetTempPath(),
            $"tr2-supervision-s4i-{Guid.NewGuid():N}.db");
        var escapedDatabasePath = databasePath.Replace("\\", "\\\\");
        var configuration = RuntimeConfigurationLoader.Parse(
            $$"""
            {
              "persistence": { "databasePath": "{{escapedDatabasePath}}" },
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
            """,
            Path.GetDirectoryName(databasePath)!);

        var runtime = PhysicalSupervisionRuntimeFactory.Create(
            configuration,
            supportedProtocolVersion: 7,
            busConnectionFactory: new FailingIfOpenedFactory());

        Assert.NotNull(runtime.Composition);
        Assert.NotNull(runtime.Operations);
        Assert.NotNull(runtime.Host);
        Assert.Equal(SupervisionRuntimeState.Created, runtime.Host.State);
        Assert.False(runtime.Composition.ReadinessGate.IsReady);
    }

    private sealed class FailingIfOpenedFactory : IModbusBusConnectionFactory
    {
        public ValueTask<IModbusBusConnection> OpenAsync(
            string busId,
            ModbusSerialConnectionSettings settings,
            CancellationToken cancellationToken = default) =>
            throw new InvalidOperationException("Factory must not open hardware during composition.");
    }
}
