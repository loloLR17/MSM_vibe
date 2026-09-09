using TR2.Application;
using TR2.Domain;
using TR2.Protocol;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class PhysicalOperationalPollingTests
{
    [Fact]
    public async Task FastMediumAndSlowPollingUsePhysicalTransportAndUpdateOnlyExistingSnapshots()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var transport = new AddressScriptedTransport();
            var composition = Compose(databasePath, new FakeFactory(transport));
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            var deviceId = new DeviceId(42);
            composition.FleetRegistry.SetSession(
                TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));

            var runner = new PhysicalPollingWorkRunner(composition, supportedProtocolVersion: 7);
            var observedAt = new DateTimeOffset(2026, 9, 9, 16, 45, 0, TimeSpan.Zero);

            await ExecuteGroupAsync(composition, runner, endpoint, PollingGroup.Fast, observedAt);

            var afterFast = composition.TelemetrySnapshotRegistry.Get(deviceId);
            Assert.True(afterFast.SystemState.HasValue);
            Assert.Equal((ushort)11, afterFast.SystemState.LastValue!.SystemStatus);
            Assert.Equal(observedAt, afterFast.SystemState.ReceivedAt);
            Assert.True(afterFast.VibrationState.HasValue);
            Assert.Equal((ushort)33, afterFast.VibrationState.LastValue!.StatusGlobal);
            Assert.Equal(observedAt, afterFast.VibrationState.ReceivedAt);
            Assert.False(afterFast.TimeState.HasValue);

            await ExecuteGroupAsync(composition, runner, endpoint, PollingGroup.Medium, observedAt.AddSeconds(1));

            var afterMedium = composition.TelemetrySnapshotRegistry.Get(deviceId);
            Assert.True(afterMedium.TimeState.HasValue);
            Assert.Equal((ushort)22, afterMedium.TimeState.LastValue!.TimeStatus);
            Assert.Equal(observedAt.AddSeconds(1), afterMedium.TimeState.ReceivedAt);

            var beforeSlow = afterMedium;
            await ExecuteGroupAsync(composition, runner, endpoint, PollingGroup.Slow, observedAt.AddSeconds(2));
            var afterSlow = composition.TelemetrySnapshotRegistry.Get(deviceId);

            Assert.Equal(beforeSlow.SystemState, afterSlow.SystemState);
            Assert.Equal(beforeSlow.TimeState, afterSlow.TimeState);
            Assert.Equal(beforeSlow.VibrationState, afterSlow.VibrationState);

            Assert.Contains(transport.Reads, read => read.StartAddress == B1SystemState.StartAddress);
            Assert.Contains(transport.Reads, read => read.StartAddress == B2TimeState.StartAddress);
            Assert.Contains(transport.Reads, read => read.StartAddress == B3VibrationSupervision.StartAddress);
            Assert.Contains(transport.Reads, read => read.StartAddress == B4ConfigurationState.StartAddress);
            Assert.Contains(transport.Reads, read => read.StartAddress == B5CommandState.StartAddress);
            Assert.Contains(transport.Reads, read => read.StartAddress == B6CampaignInventoryState.StartAddress);
            Assert.Contains(transport.Reads, read => read.StartAddress == B7DiagnosticState.StartAddress);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    private static async ValueTask ExecuteGroupAsync(
        SupervisionRuntimeComposition composition,
        PhysicalPollingWorkRunner runner,
        TR2Endpoint endpoint,
        PollingGroup group,
        DateTimeOffset observedAt)
    {
        composition.BusWorkScheduler.QueuePolling(endpoint, group, observedAt);
        var work = composition.BusWorkScheduler.BeginNext(endpoint.Bus, observedAt)
            ?? throw new InvalidOperationException("Expected queued polling work.");

        var result = await runner.ExecuteAsync(work, observedAt);
        Assert.True(result.EndpointCompatible);
    }

    private static SupervisionRuntimeComposition Compose(
        string databasePath,
        IModbusBusConnectionFactory factory)
    {
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

        return SupervisionRuntimeCompositionRoot.Compose(configuration, factory);
    }

    private static string NewDatabasePath() =>
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s4f2-{Guid.NewGuid():N}.db");

    private static void DeleteDatabaseFiles(string databasePath)
    {
        foreach (var suffix in new[] { string.Empty, "-wal", "-shm" })
        {
            var path = databasePath + suffix;
            if (File.Exists(path))
            {
                File.Delete(path);
            }
        }
    }

    private sealed class FakeFactory : IModbusBusConnectionFactory
    {
        private readonly IRegisterTransport _transport;

        public FakeFactory(IRegisterTransport transport)
        {
            _transport = transport;
        }

        public ValueTask<IModbusBusConnection> OpenAsync(
            string busId,
            ModbusSerialConnectionSettings settings,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            return ValueTask.FromResult<IModbusBusConnection>(new FakeConnection(busId, _transport));
        }
    }

    private sealed class FakeConnection : IModbusBusConnection
    {
        public FakeConnection(string busId, IRegisterTransport transport)
        {
            BusId = busId;
            RegisterTransport = transport;
            RegisterWriteTransport = new NullWriteTransport();
        }

        public string BusId { get; }
        public IRegisterTransport RegisterTransport { get; }
        public IRegisterWriteTransport RegisterWriteTransport { get; }
        public ValueTask DisposeAsync() => ValueTask.CompletedTask;
    }

    private sealed class AddressScriptedTransport : IRegisterTransport
    {
        public List<(ushort StartAddress, ushort RegisterCount)> Reads { get; } = [];

        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            Reads.Add((startAddress, registerCount));

            var registers = new ushort[registerCount];
            if (startAddress == B1SystemState.StartAddress)
            {
                registers[0] = 11;
            }
            else if (startAddress == B2TimeState.StartAddress)
            {
                registers[0] = 22;
            }
            else if (startAddress == B3VibrationSupervision.StartAddress)
            {
                registers[0] = 33;
            }

            return ValueTask.FromResult(registers);
        }
    }

    private sealed class NullWriteTransport : IRegisterWriteTransport
    {
        public ValueTask WriteRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            IReadOnlyList<ushort> values,
            CancellationToken cancellationToken = default) =>
            ValueTask.CompletedTask;
    }
}
