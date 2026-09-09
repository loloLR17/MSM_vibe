using TR2.Domain;
using TR2.Protocol;
using Xunit;

namespace TR2.Application.Tests;

public sealed class FleetReconnectTests
{
    private static readonly DateTimeOffset DueAt =
        new(2026, 9, 9, 10, 0, 0, TimeSpan.Zero);

    [Fact]
    public async Task Reconnect_rechecks_B0_before_queueing_B1_through_B7_refresh()
    {
        var endpoint = Endpoint();
        var registry = new FleetRegistry();
        registry.RegisterEndpoint(endpoint);
        registry.SetSession(
            TR2Session.CreateCompatible(endpoint, new TR2Device(new DeviceId(1001))).MarkDisconnected());
        var scheduler = new BusWorkScheduler();
        var replacement = TR2Session.CreateCompatible(
            endpoint,
            new TR2Device(new DeviceId(2002)));
        var reader = new RecordingReader(replacement);
        var service = new FleetDiscoveryService(
            registry,
            reader,
            supportedProtocolVersion: 1,
            new FleetRefreshPlanner(scheduler));

        var result = await service.ReconnectAsync(endpoint, DueAt);

        Assert.True(reader.WasRead);
        Assert.Equal(new DeviceId(2002), result.Session.Device!.DeviceId);
        Assert.Equal(TR2PollingPlan.PostReconnectRefreshBlocks, result.Refreshes.Select(item => item.Block));
        Assert.DoesNotContain(result.Refreshes, item => item.Block == TR2RegisterBlock.B0);
    }

    [Fact]
    public async Task Incompatible_B0_does_not_queue_state_refresh()
    {
        var endpoint = Endpoint();
        var registry = new FleetRegistry();
        registry.RegisterEndpoint(endpoint);
        var service = new FleetDiscoveryService(
            registry,
            new RecordingReader(TR2Session.CreateIncompatible(endpoint)),
            supportedProtocolVersion: 1,
            new FleetRefreshPlanner(new BusWorkScheduler()));

        var result = await service.ReconnectAsync(endpoint, DueAt);

        Assert.Equal(TR2SessionState.Incompatible, result.Session.State);
        Assert.Empty(result.Refreshes);
    }

    private static TR2Endpoint Endpoint() =>
        new(new SerialBus("RS485-A"), new ModbusAddress(10));

    private sealed class RecordingReader : IB0SessionReader
    {
        private readonly TR2Session _result;

        public RecordingReader(TR2Session result)
        {
            _result = result;
        }

        public bool WasRead { get; private set; }

        public ValueTask<TR2Session> ReadSessionAsync(
            TR2Endpoint endpoint,
            ushort supportedProtocolVersion,
            CancellationToken cancellationToken = default)
        {
            WasRead = true;
            return ValueTask.FromResult(_result);
        }
    }
}
