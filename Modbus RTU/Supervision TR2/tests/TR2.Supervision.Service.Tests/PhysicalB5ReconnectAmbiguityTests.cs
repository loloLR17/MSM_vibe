using TR2.Application;
using TR2.Domain;
using TR2.Protocol;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class PhysicalB5ReconnectAmbiguityTests
{
    [Fact]
    public async Task AmbiguousB5SurvivesIoReconnectAndFreshB0AndBlocksNewCommand()
    {
        var databasePath = NewDatabasePath();
        try
        {
            const ushort supportedProtocolVersion = 7;
            var firstTransport = new ScriptedTransport();
            var secondTransport = new ScriptedTransport();
            var factory = new SequencedFactory(firstTransport, secondTransport);
            var composition = Compose(databasePath, factory);
            await new SupervisionRuntimeStartup(composition).StartAsync();

            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            var deviceId = new DeviceId(42);
            composition.FleetRegistry.SetSession(TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));

            var operations = new SupervisionOperationalFacade(composition);
            var intent = new B5CommandIntent(3, 4, 5, 6, 7);
            var dueAt = new DateTimeOffset(2026, 9, 9, 18, 0, 0, TimeSpan.Zero);
            var queued = await operations.QueueCommandAsync(endpoint, "request-1", intent, dueAt);
            var work = composition.BusWorkScheduler.BeginNext(endpoint.Bus, dueAt)!;

            firstTransport.WriteFailures.Enqueue(null);
            firstTransport.WriteFailures.Enqueue(new ModbusTransportFailureException(
                ModbusTransportFailureKind.Io,
                "port removed",
                new IOException("port removed")));

            var priorityRunner = new PhysicalPriorityWorkRunner(composition, operations);
            await priorityRunner.ExecuteAsync(work, dueAt);

            var coordinator = composition.CommandCoordinatorRegistry.Get(deviceId);
            Assert.NotNull(coordinator.ActiveTransaction);
            Assert.Equal(CommandTransactionState.Ambiguous, coordinator.ActiveTransaction!.State);
            Assert.False(composition.BusConnectionManager.TryGet(endpoint.Bus.Id, out _));
            Assert.Equal(TR2SessionState.Unidentified, composition.FleetRegistry.GetSession(endpoint).State);

            var reconnect = new SerialBusReconnectScheduler(composition, TimeSpan.FromSeconds(5));
            await reconnect.RunDueAsync(dueAt);
            await reconnect.RunDueAsync(dueAt + TimeSpan.FromSeconds(5));

            Assert.True(composition.BusConnectionManager.TryGet(endpoint.Bus.Id, out _));
            Assert.Equal(TR2SessionState.Unidentified, composition.FleetRegistry.GetSession(endpoint).State);
            Assert.Equal(CommandTransactionState.Ambiguous, coordinator.ActiveTransaction!.State);

            var b0 = new ushort[B0Identification.RegisterCount];
            b0[B0Identification.DeviceIdLswOffset] = 42;
            b0[B0Identification.ProtocolVersionOffset] = supportedProtocolVersion;
            secondTransport.ReadScripts[B0Reader.StartAddress] = b0;

            var b0At = dueAt + TimeSpan.FromSeconds(6);
            composition.BusWorkScheduler.QueuePolling(endpoint, PollingGroup.Static, b0At);
            var b0Work = composition.BusWorkScheduler.BeginNext(endpoint.Bus, b0At)!;
            var pollingRunner = new PhysicalPollingWorkRunner(composition, supportedProtocolVersion);
            var b0Result = await pollingRunner.ExecuteAsync(b0Work, b0At);

            Assert.True(b0Result.EndpointCompatible);
            Assert.Equal(TR2SessionState.Compatible, composition.FleetRegistry.GetSession(endpoint).State);
            Assert.Equal(CommandTransactionState.Ambiguous, coordinator.ActiveTransaction!.State);

            await Assert.ThrowsAsync<InvalidOperationException>(async () =>
                await operations.QueueCommandAsync(
                    endpoint,
                    "request-2",
                    intent,
                    b0At + TimeSpan.FromSeconds(1)));

            Assert.Equal(CommandTransactionState.Ambiguous, coordinator.ActiveTransaction!.State);
            Assert.Equal(new TransactionId(queued.Request.TransactionId), coordinator.ActiveTransaction.TransactionId);
            Assert.Empty(secondTransport.Writes);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
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
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s4g3-{Guid.NewGuid():N}.db");

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

    private sealed class SequencedFactory : IModbusBusConnectionFactory
    {
        private readonly Queue<ScriptedTransport> _transports;

        public SequencedFactory(params ScriptedTransport[] transports)
        {
            _transports = new Queue<ScriptedTransport>(transports);
        }

        public ValueTask<IModbusBusConnection> OpenAsync(
            string busId,
            ModbusSerialConnectionSettings settings,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            if (_transports.Count == 0)
            {
                throw new InvalidOperationException("No scripted transport remains for this open request.");
            }

            var transport = _transports.Dequeue();
            return ValueTask.FromResult<IModbusBusConnection>(new FakeConnection(busId, transport));
        }
    }

    private sealed class FakeConnection : IModbusBusConnection
    {
        public FakeConnection(string busId, ScriptedTransport transport)
        {
            BusId = busId;
            RegisterTransport = transport;
            RegisterWriteTransport = transport;
        }

        public string BusId { get; }
        public IRegisterTransport RegisterTransport { get; }
        public IRegisterWriteTransport RegisterWriteTransport { get; }
        public ValueTask DisposeAsync() => ValueTask.CompletedTask;
    }

    private sealed class ScriptedTransport : IRegisterTransport, IRegisterWriteTransport
    {
        public Dictionary<ushort, ushort[]> ReadScripts { get; } = [];
        public Queue<Exception?> WriteFailures { get; } = new();
        public List<(ushort StartAddress, ushort[] Values)> Writes { get; } = [];

        public ValueTask<ushort[]> ReadRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            ushort registerCount,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            if (!ReadScripts.TryGetValue(startAddress, out var registers))
            {
                throw new InvalidOperationException($"No read script is registered for address {startAddress}.");
            }

            return ValueTask.FromResult(registers.Take(registerCount).ToArray());
        }

        public ValueTask WriteRegistersAsync(
            string busId,
            byte unitAddress,
            ushort startAddress,
            IReadOnlyList<ushort> values,
            CancellationToken cancellationToken = default)
        {
            cancellationToken.ThrowIfCancellationRequested();
            Writes.Add((startAddress, values.ToArray()));

            if (WriteFailures.Count > 0)
            {
                var failure = WriteFailures.Dequeue();
                if (failure is not null)
                {
                    return ValueTask.FromException(failure);
                }
            }

            return ValueTask.CompletedTask;
        }
    }
}
