using TR2.Domain;
using TR2.Protocol;
using TR2.Transport;
using Xunit;

namespace TR2.Application.Tests;

public sealed class PollingTelemetryCycleTests
{
    private static readonly DateTimeOffset Now =
        new(2026, 9, 9, 14, 0, 0, TimeSpan.Zero);

    [Fact]
    public async Task Classified_communication_failure_preserves_last_values_and_marks_unavailable()
    {
        var endpoint = Endpoint();
        var fleet = CompatibleFleet(endpoint, new DeviceId(1001));
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var previous = B1();
        telemetry.ReceiveSystemState(new DeviceId(1001), previous, Now.AddSeconds(-1));

        var scheduler = new BusWorkScheduler();
        var polling = new PollingBusOrchestrator(
            scheduler,
            new PollingExecutor(
                new FailingTransport(new IOException("Injected communication failure.")),
                supportedProtocolVersion: 1));
        var cycle = new PollingTelemetryCycle(
            polling,
            new PollingTelemetryPublisher(fleet, telemetry),
            new IOExceptionFailureClassifier());

        polling.Queue(endpoint, PollingGroup.Fast, Now);
        var active = polling.BeginNext(endpoint.Bus, Now)!;

        await Assert.ThrowsAsync<IOException>(async () =>
            await cycle.ExecuteAsync(active, Now));

        var snapshots = telemetry.Get(new DeviceId(1001));
        Assert.Equal(previous, snapshots.SystemState.LastValue);
        Assert.False(snapshots.SystemState.IsAvailable);
        Assert.Null(scheduler.BeginNext(endpoint.Bus, Now));
    }

    [Fact]
    public async Task Unclassified_failure_does_not_change_snapshot_availability()
    {
        var endpoint = Endpoint();
        var fleet = CompatibleFleet(endpoint, new DeviceId(1001));
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        telemetry.ReceiveSystemState(new DeviceId(1001), B1(), Now.AddSeconds(-1));

        var scheduler = new BusWorkScheduler();
        var polling = new PollingBusOrchestrator(
            scheduler,
            new PollingExecutor(
                new FailingTransport(new InvalidOperationException("Injected non-communication failure.")),
                supportedProtocolVersion: 1));
        var cycle = new PollingTelemetryCycle(
            polling,
            new PollingTelemetryPublisher(fleet, telemetry),
            new IOExceptionFailureClassifier());

        polling.Queue(endpoint, PollingGroup.Fast, Now);
        var active = polling.BeginNext(endpoint.Bus, Now)!;

        await Assert.ThrowsAsync<InvalidOperationException>(async () =>
            await cycle.ExecuteAsync(active, Now));

        Assert.True(telemetry.Get(new DeviceId(1001)).SystemState.IsAvailable);
        Assert.Null(scheduler.BeginNext(endpoint.Bus, Now));
    }

    [Fact]
    public async Task Successful_polling_cycle_publishes_telemetry_normally()
    {
        var endpoint = Endpoint();
        var fleet = CompatibleFleet(endpoint, new DeviceId(1001));
        var telemetry = new DeviceTelemetrySnapshotRegistry();
        var scheduler = new BusWorkScheduler();
        var polling = new PollingBusOrchestrator(
            scheduler,
            new PollingExecutor(new ZeroTransport(), supportedProtocolVersion: 1));
        var cycle = new PollingTelemetryCycle(
            polling,
            new PollingTelemetryPublisher(fleet, telemetry),
            new IOExceptionFailureClassifier());

        polling.Queue(endpoint, PollingGroup.Fast, Now);
        var active = polling.BeginNext(endpoint.Bus, Now)!;
        var published = await cycle.ExecuteAsync(active, Now);

        Assert.NotNull(published);
        Assert.True(published!.SystemState.HasValue);
        Assert.True(published.VibrationState.HasValue);
        Assert.Equal(Now, published.SystemState.ReceivedAt);
        Assert.Equal(Now, published.VibrationState.ReceivedAt);
        Assert.Null(scheduler.BeginNext(endpoint.Bus, Now));
    }

    private static FleetRegistry CompatibleFleet(TR2Endpoint endpoint, DeviceId deviceId)
    {
        var fleet = new FleetRegistry();
        fleet.RegisterEndpoint(endpoint);
        fleet.SetSession(TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));
        return fleet;
    }

    private static TR2Endpoint Endpoint() =>
        new(new SerialBus("RS485-A"), new ModbusAddress(10));

    private static B1SystemState B1() =>
        new(1, 0, 0, 0, 100, 1, 250, 10, 20, 1, 30, 1, 42, 0, 0);

    private sealed class IOExceptionFailureClassifier : IPollingFailureClassifier
    {
        public bool IsCommunicationFailure(Exception exception) => exception is IOException;
    }

    private sealed class FailingTransport(Exception exception) : IRegisterTransport
    {
        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromException<ushort[]>(exception);
    }

    private sealed class ZeroTransport : IRegisterTransport
    {
        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromResult(new ushort[registerCount]);
    }
}
