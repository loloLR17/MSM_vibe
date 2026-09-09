using Microsoft.Data.Sqlite;
using TR2.Domain;
using TR2.Persistence.Sqlite;
using Xunit;

namespace TR2.Persistence.Tests;

public sealed class SqliteEquipmentModelStoreTests : IDisposable
{
    private readonly string _directory = Path.Combine(Path.GetTempPath(), $"tr2-equipment-model-{Guid.NewGuid():N}");
    private static readonly DateTimeOffset Start = new(2026, 9, 9, 8, 0, 0, TimeSpan.Zero);

    [Fact]
    public async Task Catalog_SurvivesDatabaseReopenWithHierarchyIntact()
    {
        var databasePath = Path.Combine(_directory, "catalog-reopen.db");
        var firstDatabase = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        var firstStore = new SqliteEquipmentModelStore(firstDatabase);

        await firstStore.AddInstallationAsync(new Installation(new InstallationId("MSM"), "Mont St Michel"));
        await firstStore.AddEquipmentAsync(new Equipment(new EquipmentId("CHILLER-1"), new InstallationId("MSM"), "Chiller 1"));
        await firstStore.AddMeasurementPointAsync(new MeasurementPoint(new MeasurementPointId("CH1-MOTOR-DE"), new EquipmentId("CHILLER-1"), "Motor DE"));

        SqliteConnection.ClearAllPools();

        var reopenedDatabase = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        var reopenedStore = new SqliteEquipmentModelStore(reopenedDatabase);
        var installations = await reopenedStore.ReadInstallationsAsync();
        var equipment = await reopenedStore.ReadEquipmentAsync();
        var points = await reopenedStore.ReadMeasurementPointsAsync();

        var installation = Assert.Single(installations);
        Assert.Equal("MSM", installation.Id.Value);
        Assert.Equal("Mont St Michel", installation.Name);

        var machine = Assert.Single(equipment);
        Assert.Equal("CHILLER-1", machine.Id.Value);
        Assert.Equal("MSM", machine.InstallationId.Value);
        Assert.Equal("Chiller 1", machine.Name);

        var point = Assert.Single(points);
        Assert.Equal("CH1-MOTOR-DE", point.Id.Value);
        Assert.Equal("CHILLER-1", point.EquipmentId.Value);
        Assert.Equal("Motor DE", point.Name);
    }

    [Fact]
    public async Task AssignmentHistory_MoveAndUnassign_SurvivesRestart()
    {
        var databasePath = Path.Combine(_directory, "assignment-restart.db");
        var firstDatabase = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        var firstStore = new SqliteEquipmentModelStore(firstDatabase);
        await AddCatalogAsync(firstStore);

        var deviceId = new DeviceId(1001);
        var firstPoint = new MeasurementPointId("P-01");
        var secondPoint = new MeasurementPointId("P-02");
        await firstStore.AssignAsync(deviceId, firstPoint, Start);
        var movedAt = Start.AddHours(2);
        await firstStore.MoveAsync(deviceId, secondPoint, movedAt);

        SqliteConnection.ClearAllPools();

        var reopenedDatabase = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        var reopenedStore = new SqliteEquipmentModelStore(reopenedDatabase);
        var beforeUnassign = await reopenedStore.ReadAssignmentHistoryAsync();

        Assert.Equal(2, beforeUnassign.Count);
        Assert.Equal(firstPoint, beforeUnassign[0].MeasurementPointId);
        Assert.Equal(movedAt.UtcDateTime, beforeUnassign[0].ValidTo!.Value.UtcDateTime);
        Assert.Equal(secondPoint, beforeUnassign[1].MeasurementPointId);
        Assert.True(beforeUnassign[1].IsActive);

        var unassignedAt = movedAt.AddHours(1);
        await reopenedStore.UnassignAsync(deviceId, unassignedAt);

        SqliteConnection.ClearAllPools();

        var secondReopen = new SqliteEquipmentModelStore(
            new SqliteDatabase(new SqlitePersistenceOptions(databasePath)));
        var finalHistory = await secondReopen.ReadAssignmentHistoryAsync();

        Assert.Equal(2, finalHistory.Count);
        Assert.False(finalHistory[0].IsActive);
        Assert.False(finalHistory[1].IsActive);
        Assert.Equal(unassignedAt.UtcDateTime, finalHistory[1].ValidTo!.Value.UtcDateTime);
    }

    [Fact]
    public async Task Assign_EnforcesSingleActiveDeviceAndSingleActivePoint()
    {
        var store = new SqliteEquipmentModelStore(CreateDatabase("assignment-uniqueness.db"));
        await AddCatalogAsync(store);

        await store.AssignAsync(new DeviceId(1001), new MeasurementPointId("P-01"), Start);

        await Assert.ThrowsAsync<InvalidOperationException>(async () =>
            await store.AssignAsync(new DeviceId(1001), new MeasurementPointId("P-02"), Start.AddMinutes(1)));
        await Assert.ThrowsAsync<InvalidOperationException>(async () =>
            await store.AssignAsync(new DeviceId(2002), new MeasurementPointId("P-01"), Start.AddMinutes(1)));
    }

    [Fact]
    public async Task MoveAndUnassign_RejectRetroactiveClosureWithoutChangingHistory()
    {
        var store = new SqliteEquipmentModelStore(CreateDatabase("assignment-time.db"));
        await AddCatalogAsync(store);
        var device = new DeviceId(1001);
        await store.AssignAsync(device, new MeasurementPointId("P-01"), Start);

        await Assert.ThrowsAsync<ArgumentOutOfRangeException>(async () =>
            await store.MoveAsync(device, new MeasurementPointId("P-02"), Start));
        await Assert.ThrowsAsync<ArgumentOutOfRangeException>(async () =>
            await store.UnassignAsync(device, Start.AddTicks(-1)));

        var history = await store.ReadAssignmentHistoryAsync();
        var active = Assert.Single(history);
        Assert.True(active.IsActive);
        Assert.Equal(new MeasurementPointId("P-01"), active.MeasurementPointId);
    }

    [Fact]
    public async Task Catalog_ForeignKeysRejectOrphanEquipmentAndMeasurementPoint()
    {
        var store = new SqliteEquipmentModelStore(CreateDatabase("catalog-foreign-key.db"));

        await Assert.ThrowsAsync<SqliteException>(async () =>
            await store.AddEquipmentAsync(new Equipment(
                new EquipmentId("E-ORPHAN"),
                new InstallationId("MISSING"),
                "Orphan")));

        await store.AddInstallationAsync(new Installation(new InstallationId("I-01"), "Installation"));
        await Assert.ThrowsAsync<SqliteException>(async () =>
            await store.AddMeasurementPointAsync(new MeasurementPoint(
                new MeasurementPointId("P-ORPHAN"),
                new EquipmentId("MISSING"),
                "Orphan point")));
    }

    [Fact]
    public void Database_MigratesExistingVersion4ToEquipmentSchema()
    {
        var databasePath = Path.Combine(_directory, "migration-v4.db");
        Directory.CreateDirectory(_directory);

        using (var connection = new SqliteConnection($"Data Source={databasePath}"))
        {
            connection.Open();
            using var command = connection.CreateCommand();
            command.CommandText = "PRAGMA user_version = 4;";
            command.ExecuteNonQuery();
        }

        var database = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        using var migrated = database.OpenConnection();

        Assert.Equal(SqliteDatabase.CurrentSchemaVersion, ReadInt32(migrated, "PRAGMA user_version;"));
        Assert.Equal(1, ReadInt32(migrated, "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='installation';"));
        Assert.Equal(1, ReadInt32(migrated, "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='equipment';"));
        Assert.Equal(1, ReadInt32(migrated, "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='measurement_point';"));
        Assert.Equal(1, ReadInt32(migrated, "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='equipment_assignment';"));
    }

    public void Dispose()
    {
        SqliteConnection.ClearAllPools();
        if (Directory.Exists(_directory))
            Directory.Delete(_directory, recursive: true);
    }

    private SqliteDatabase CreateDatabase(string fileName) =>
        new(new SqlitePersistenceOptions(Path.Combine(_directory, fileName)));

    private static async Task AddCatalogAsync(SqliteEquipmentModelStore store)
    {
        await store.AddInstallationAsync(new Installation(new InstallationId("I-01"), "Installation"));
        await store.AddEquipmentAsync(new Equipment(new EquipmentId("E-01"), new InstallationId("I-01"), "Equipment"));
        await store.AddMeasurementPointAsync(new MeasurementPoint(new MeasurementPointId("P-01"), new EquipmentId("E-01"), "Point 1"));
        await store.AddMeasurementPointAsync(new MeasurementPoint(new MeasurementPointId("P-02"), new EquipmentId("E-01"), "Point 2"));
    }

    private static int ReadInt32(SqliteConnection connection, string sql)
    {
        using var command = connection.CreateCommand();
        command.CommandText = sql;
        return Convert.ToInt32(command.ExecuteScalar(), System.Globalization.CultureInfo.InvariantCulture);
    }
}
