using TR2.Domain;
using TR2.Transport;
using Xunit;

namespace TR2.Application.Tests;

public sealed class PollingExecutionTests
{
    private static readonly DateTimeOffset Now =
        new(2026, 9, 9, 13, 0, 0, TimeSpan.Zero);

    [Theory]
    [InlineData(PollingGroup.Fast, 1000, 3000, 5000)]
    [InlineData(PollingGroup.Medium, 2000, 7000)]
    [InlineData(PollingGroup.Slow, 4000, 6000)]
    [InlineData(PollingGroup.Static, 0)]
    public async Task Polling_group_reads_exact_S0_blocks(
        PollingGroup group,
        params int[] expectedStartAddresses)
    {
        var transport = new RecordingTransport();
        var scheduler = new BusWorkScheduler();
        var executor = new PollingExecutor(transport, supportedProtocolVersion: 1);
        var orchestrator = new PollingBusOrchestrator(scheduler, executor);
        var endpoint = Endpoint();

        var queued = orchestrator.Queue(endpoint, group, Now);
        var active = orchestrator.BeginNext(endpoint.Bus, Now)!;
        var result = await orchestrator.ExecuteAsync(active);

        Assert.Equal(queued, active);
        Assert.Equal(group, result.Group);
        Assert.Equal(expectedStartAddresses, transport.StartAddresses.Select(value => (int)value));
        Assert.Null(scheduler.BeginNext(endpoint.Bus, Now));
    }

    [Fact]
    public async Task Static_polling_uses_B0_session_compatibility_authority()
    {
        var transport = new RecordingTransport();
        var scheduler = new BusWorkScheduler();
        var orchestrator = new PollingBusOrchestrator(
            scheduler,
            new PollingExecutor(transport, supportedProtocolVersion: 1));
        var endpoint = Endpoint();

        var queued = orchestrator.Queue(endpoint, PollingGroup.Static, Now);
        var active = orchestrator.BeginNext(endpoint.Bus, Now)!;
        var result = await orchestrator.ExecuteAsync(active);

        Assert.Equal(queued, active);
        Assert.NotNull(result.B0Session);
        Assert.Equal(TR2SessionState.Compatible, result.B0Session!.State);
        Assert.Equal((uint)1001, result.B0Session.Device!.DeviceId.Value);
    }

    [Fact]
    public async Task Priority_command_still_preempts_due_polling_on_shared_scheduler()
    {
        var scheduler = new BusWorkScheduler();
        var polling = new PollingBusOrchestrator(
            scheduler,
            new PollingExecutor(new RecordingTransport(), supportedProtocolVersion: 1));
        var commands = new CommandBusOrchestrator(scheduler);
        var endpoint = Endpoint();
        var coordinator = new CommandCoordinator(
            new DeviceId(1001),
            new ReservationStore());

        polling.Queue(endpoint, PollingGroup.Fast, Now.AddMinutes(-1));
        var command = await commands.PrepareAndQueueAsync(
            endpoint,
            coordinator,
            "START",
            Now);

        Assert.Equal(command, polling.BeginNext(endpoint.Bus, Now));
    }

    [Fact]
    public async Task Polling_transport_failure_releases_bus_and_propagates()
    {
        var transport = new RecordingTransport { FailAtStartAddress = 3000 };
        var scheduler = new BusWorkScheduler();
        var orchestrator = new PollingBusOrchestrator(
            scheduler,
            new PollingExecutor(transport, supportedProtocolVersion: 1));
        var endpoint = Endpoint();

        orchestrator.Queue(endpoint, PollingGroup.Fast, Now);
        var active = orchestrator.BeginNext(endpoint.Bus, Now)!;

        await Assert.ThrowsAsync<IOException>(async () =>
            await orchestrator.ExecuteAsync(active));

        var next = scheduler.QueuePolling(endpoint, PollingGroup.Fast, Now);
        Assert.Equal(next, scheduler.BeginNext(endpoint.Bus, Now));
    }

    private static TR2Endpoint Endpoint() =>
        new(new SerialBus("RS485-A"), new ModbusAddress(10));

    private sealed class RecordingTransport : IRegisterTransport
    {
        public List<ushort> StartAddresses { get; } = [];
        public ushort? FailAtStartAddress { get; init; }

        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default)
        {
            StartAddresses.Add(startAddress);

            if (FailAtStartAddress == startAddress)
            {
                return ValueTask.FromException<ushort[]>(new IOException("Injected polling read failure."));
            }

            var registers = new ushort[registerCount];
            if (startAddress == 0 && registerCount >= 7)
            {
                registers[0] = 0;
                registers[1] = 1001;
                registers[6] = 1;
            }

            return ValueTask.FromResult(registers);
        }
    }

    private sealed class ReservationStore : ICommandTransactionReservationStore
    {
        private TransactionId? _last;

        public ValueTask<TransactionId?> GetLastAllocatedAsync(
            DeviceId deviceId,
            CancellationToken cancellationToken = default) =>
            ValueTask.FromResult(_last);

        public ValueTask PersistAllocationAsync(
            DeviceId deviceId,
            TransactionId transactionId,
            CancellationToken cancellationToken = default)
        {
            _last = transactionId;
            return ValueTask.CompletedTask;
        }
    }
}
