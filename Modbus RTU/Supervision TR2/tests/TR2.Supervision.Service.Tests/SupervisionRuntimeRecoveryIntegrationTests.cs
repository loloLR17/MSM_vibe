using TR2.Application;
using TR2.Domain;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class SupervisionRuntimeRecoveryIntegrationTests
{
    [Fact]
    public async Task RestartRestoresAmbiguousCommandAndKeepsOperationalBarrierSemantics()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var first = Compose(databasePath);
            await new SupervisionRuntimeStartup(first).StartAsync();

            var endpoint = first.Configuration.Buses.Single().Endpoints.Single();
            var deviceId = new DeviceId(4242);
            first.FleetRegistry.SetSession(
                TR2Session.CreateCompatible(endpoint, new TR2Device(deviceId)));

            var facade = new SupervisionOperationalFacade(first);
            var queued = await facade.QueueCommandAsync(
                endpoint,
                "restart-integration",
                new B5CommandIntent(5, 1, 2, 3, 4),
                DateTimeOffset.UtcNow);

            var firstCoordinator = first.CommandCoordinatorRegistry.Get(deviceId);
            await firstCoordinator.MarkSubmittedAsync(DateTimeOffset.UtcNow);
            await firstCoordinator.MarkAmbiguousAsync(DateTimeOffset.UtcNow);

            var originalTransactionId = firstCoordinator.ActiveTransaction!.TransactionId;
            Assert.Equal(queued.Request.TransactionId, originalTransactionId.Value);

            var second = Compose(databasePath);
            var beforeStartup = new SupervisionRuntimeStatusReader(second).Read();
            Assert.False(beforeStartup.IsReady);
            Assert.Equal(0, beforeStartup.CoordinatorCount);

            await new SupervisionRuntimeStartup(second).StartAsync();

            var recovered = second.CommandCoordinatorRegistry.Get(deviceId);
            Assert.NotNull(recovered.ActiveTransaction);
            Assert.Equal(CommandTransactionState.Ambiguous, recovered.ActiveTransaction!.State);
            Assert.Equal(originalTransactionId, recovered.ActiveTransaction.TransactionId);

            var status = new SupervisionRuntimeStatusReader(second).Read();
            Assert.True(status.IsReady);
            Assert.Equal(1, status.EndpointCount);
            Assert.Equal(1, status.CoordinatorCount);
            Assert.Equal(1, status.AmbiguousCommandCount);

            var secondEndpoint = second.Configuration.Buses.Single().Endpoints.Single();
            second.FleetRegistry.SetSession(
                TR2Session.CreateCompatible(secondEndpoint, new TR2Device(deviceId)));

            var secondFacade = new SupervisionOperationalFacade(second);
            await Assert.ThrowsAsync<InvalidOperationException>(async () =>
                await secondFacade.QueueCommandAsync(
                    secondEndpoint,
                    "must-be-blocked-by-ambiguous",
                    new B5CommandIntent(6, 0, 0, 0, 0),
                    DateTimeOffset.UtcNow));
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    [Fact]
    public async Task StatusReaderReflectsFleetAndReadinessWithoutSideEffects()
    {
        var databasePath = NewDatabasePath();
        try
        {
            var composition = Compose(databasePath);
            var statusReader = new SupervisionRuntimeStatusReader(composition);

            var initial = statusReader.Read();
            Assert.False(initial.IsReady);
            Assert.Equal(1, initial.EndpointCount);
            Assert.Equal(0, initial.CompatibleEndpointCount);
            Assert.Equal(0, initial.DisconnectedEndpointCount);

            await new SupervisionRuntimeStartup(composition).StartAsync();
            var endpoint = composition.Configuration.Buses.Single().Endpoints.Single();
            composition.FleetRegistry.SetSession(
                TR2Session.CreateCompatible(endpoint, new TR2Device(new DeviceId(100))));

            var running = statusReader.Read();
            Assert.True(running.IsReady);
            Assert.Equal(1, running.CompatibleEndpointCount);
            Assert.Equal(0, running.DisconnectedEndpointCount);

            composition.FleetRegistry.SetSession(
                composition.FleetRegistry.GetSession(endpoint).MarkDisconnected());

            var disconnected = statusReader.Read();
            Assert.Equal(0, disconnected.CompatibleEndpointCount);
            Assert.Equal(1, disconnected.DisconnectedEndpointCount);
        }
        finally
        {
            DeleteDatabaseFiles(databasePath);
        }
    }

    private static SupervisionRuntimeComposition Compose(string databasePath)
    {
        var configuration = RuntimeConfigurationLoader.Parse(
            $$"""
            {
              "persistence": { "databasePath": "{{databasePath.Replace("\\", "\\\\")}}" },
              "buses": [
                { "id": "bus-1", "endpoints": [1] }
              ]
            }
            """,
            Path.GetDirectoryName(databasePath)!);

        return SupervisionRuntimeCompositionRoot.Compose(configuration);
    }

    private static string NewDatabasePath() =>
        Path.Combine(Path.GetTempPath(), $"tr2-supervision-s3g-{Guid.NewGuid():N}.db");

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
}
