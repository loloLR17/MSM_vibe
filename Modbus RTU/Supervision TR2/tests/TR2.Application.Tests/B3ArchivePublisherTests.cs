using TR2.Domain;
using TR2.Protocol;
using Xunit;

namespace TR2.Application.Tests;

public sealed class B3ArchivePublisherTests
{
    private static readonly DateTimeOffset ReceivedAt =
        new(2026, 9, 9, 16, 0, 0, TimeSpan.Zero);

    [Fact]
    public async Task B3_is_archived_with_durable_device_identity_and_PC_receive_time()
    {
        var endpoint = Endpoint();
        var deviceId = new DeviceId(1001);
        var fleet = CompatibleFleet(endpoint, deviceId);
        var sink = new RecordingSink();
        var publisher = new B3ArchivePublisher(fleet, sink);
        var b3 = B3();

        var archived = await publisher.ArchiveAsync(
            endpoint,
            ReadSet(b3),
            ReceivedAt);

        Assert.NotNull(archived);
        Assert.Equal(deviceId, archived!.DeviceId);
        Assert.Equal(b3, archived.Value);
        Assert.Equal(ReceivedAt, archived.ReceivedAt);
        Assert.Equal(archived, sink.Observations.Single());
    }

    [Fact]
    public async Task Read_set_without_B3_does_not_append_archive_record()
    {
        var endpoint = Endpoint();
        var sink = new RecordingSink();
        var publisher = new B3ArchivePublisher(
            CompatibleFleet(endpoint, new DeviceId(1001)),
            sink);

        var archived = await publisher.ArchiveAsync(
            endpoint,
            new PollingReadSet(PollingGroup.Medium, null, null, null, null, null, null, null, null),
            ReceivedAt);

        Assert.Null(archived);
        Assert.Empty(sink.Observations);
    }

    [Fact]
    public async Task B3_archiving_requires_current_compatible_identified_session()
    {
        var endpoint = Endpoint();
        var fleet = new FleetRegistry();
        fleet.RegisterEndpoint(endpoint);
        var publisher = new B3ArchivePublisher(fleet, new RecordingSink());

        await Assert.ThrowsAsync<InvalidOperationException>(async () =>
            await publisher.ArchiveAsync(endpoint, ReadSet(B3()), ReceivedAt));
    }

    [Fact]
    public async Task Archive_sink_failure_is_not_hidden()
    {
        var endpoint = Endpoint();
        var publisher = new B3ArchivePublisher(
            CompatibleFleet(endpoint, new DeviceId(1001)),
            new FailingSink());

        var error = await Assert.ThrowsAsync<IOException>(async () =>
            await publisher.ArchiveAsync(endpoint, ReadSet(B3()), ReceivedAt));

        Assert.Equal("Injected archive failure.", error.Message);
    }

    private static PollingReadSet ReadSet(B3VibrationSupervision b3) =>
        new(PollingGroup.Fast, null, null, null, b3, null, null, null, null);

    private static B3VibrationSupervision B3() =>
        new(3, 1, 2, 1, 1000, 50, 7, 1000, 100, 10, 20, 1, 2, 3, 4, 5, 6, 1, 0, 0, 0, 0, 0, 8, 9);

    private static FleetRegistry CompatibleFleet(TR2Endpoint endpoint, DeviceId deviceId)
    {
        var fleet = new FleetRegistry();
        fleet.RegisterEndpoint(endpoint);
        fleet.SetSession(TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));
        return fleet;
    }

    private static TR2Endpoint Endpoint() =>
        new(new SerialBus("RS485-A"), new ModbusAddress(10));

    private sealed class RecordingSink : IB3ArchiveSink
    {
        public List<B3ArchiveObservation> Observations { get; } = [];

        public ValueTask AppendAsync(
            B3ArchiveObservation observation,
            CancellationToken cancellationToken = default)
        {
            Observations.Add(observation);
            return ValueTask.CompletedTask;
        }
    }

    private sealed class FailingSink : IB3ArchiveSink
    {
        public ValueTask AppendAsync(
            B3ArchiveObservation observation,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromException(new IOException("Injected archive failure."));
    }
}
