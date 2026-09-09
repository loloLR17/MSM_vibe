using Xunit;
using TR2.Domain;
using TR2.Supervision.Service;

namespace TR2.Supervision.Service.Tests;

public sealed class SupervisionRuntimeCompositionRootTests
{
    [Fact]
    public void ComposeRegistersConfiguredEndpointsWithoutOpeningDatabase()
    {
        var root = Path.Combine(Path.GetTempPath(), $"tr2-s3b-{Guid.NewGuid():N}");
        Directory.CreateDirectory(root);
        try
        {
            var configuration = RuntimeConfigurationLoader.Parse(
                """
                {
                  "persistence": { "databasePath": "data/tr2.db" },
                  "buses": [
                    { "id": "bus-a", "endpoints": [1, 2] },
                    { "id": "bus-b", "endpoints": [7] }
                  ]
                }
                """,
                root);

            var composition = SupervisionRuntimeCompositionRoot.Compose(configuration);

            Assert.Same(configuration, composition.Configuration);
            Assert.Equal(3, composition.FleetRegistry.Sessions.Count);
            Assert.All(composition.FleetRegistry.Sessions, session =>
                Assert.Equal(TR2SessionState.Unidentified, session.State));
            Assert.Contains(composition.FleetRegistry.Sessions, session =>
                session.Endpoint.Bus.Id == "bus-a" && session.Endpoint.Address == new ModbusAddress(2));
            Assert.Contains(composition.FleetRegistry.Sessions, session =>
                session.Endpoint.Bus.Id == "bus-b" && session.Endpoint.Address == new ModbusAddress(7));
            Assert.Empty(composition.CommandCoordinatorRegistry.Coordinators);
            Assert.False(File.Exists(configuration.Persistence.DatabasePath));
        }
        finally
        {
            if (Directory.Exists(root))
            {
                Directory.Delete(root, recursive: true);
            }
        }
    }

    [Fact]
    public void ComposeCreatesSharedRuntimeAuthorities()
    {
        var root = Path.Combine(Path.GetTempPath(), $"tr2-s3b-{Guid.NewGuid():N}");
        Directory.CreateDirectory(root);
        try
        {
            var configuration = RuntimeConfigurationLoader.Parse(
                """
                {
                  "persistence": { "databasePath": "tr2.db" },
                  "buses": []
                }
                """,
                root);

            var composition = SupervisionRuntimeCompositionRoot.Compose(configuration);

            Assert.NotNull(composition.Database);
            Assert.NotNull(composition.CommandReservationStore);
            Assert.NotNull(composition.CommandJournal);
            Assert.NotNull(composition.B3ArchiveSink);
            Assert.NotNull(composition.CommunicationJournalSink);
            Assert.NotNull(composition.EquipmentModelStore);
            Assert.NotNull(composition.FleetRegistry);
            Assert.NotNull(composition.CommandCoordinatorRegistry);
            Assert.NotNull(composition.TelemetrySnapshotRegistry);
            Assert.NotNull(composition.BusWorkScheduler);
            Assert.False(File.Exists(configuration.Persistence.DatabasePath));
        }
        finally
        {
            if (Directory.Exists(root))
            {
                Directory.Delete(root, recursive: true);
            }
        }
    }
}
