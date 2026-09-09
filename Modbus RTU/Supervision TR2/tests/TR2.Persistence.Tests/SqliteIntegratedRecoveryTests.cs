using System.Globalization;
using Microsoft.Data.Sqlite;
using TR2.Application;
using TR2.Domain;
using TR2.Persistence.Sqlite;
using TR2.Protocol;
using Xunit;

namespace TR2.Persistence.Tests;

public sealed class SqliteIntegratedRecoveryTests : IDisposable
{
    private readonly string _directory = Path.Combine(
        Path.GetTempPath(),
        $"tr2-integrated-recovery-{Guid.NewGuid():N}");

    private static readonly DateTimeOffset Start =
        new(2026, 9, 9, 12, 0, 0, TimeSpan.Zero);

    [Fact]
    public async Task EmptyDatabase_ReopensWithoutPhantomRecoveredState()
    {
        var databasePath = Path.Combine(_directory, "empty.db");
        var database = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        using (database.OpenConnection())
        {
        }

        SqliteConnection.ClearAllPools();

        var reopened = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        var reservationStore = new SqliteCommandTransactionReservationStore(reopened);
        var journal = new SqliteCommandTransactionJournal(reopened);
        var equipmentStore = new SqliteEquipmentModelStore(reopened);
        var deviceId = new DeviceId(1001);
        var coordinator = new CommandCoordinator(deviceId, reservationStore, journal);
        var recovery = new CommandCoordinatorRecoveryService(journal);

        Assert.Null(await reservationStore.GetLastAllocatedAsync(deviceId));
        Assert.Null(await recovery.RestoreAsync(coordinator));
        Assert.Null(coordinator.ActiveTransaction);
        Assert.Empty(await equipmentStore.ReadInstallationsAsync());
        Assert.Empty(await equipmentStore.ReadEquipmentAsync());
        Assert.Empty(await equipmentStore.ReadMeasurementPointsAsync());
        Assert.Empty(await equipmentStore.ReadAssignmentHistoryAsync());

        using var connection = reopened.OpenConnection();
        Assert.Equal(0, ReadInt32(connection, "SELECT COUNT(*) FROM b3_archive;"));
        Assert.Equal(0, ReadInt32(connection, "SELECT COUNT(*) FROM communication_failure_journal;"));
    }

    [Fact]
    public async Task FullDurableState_ReopensAndRecoversB5WithoutLosingOtherStores()
    {
        var databasePath = Path.Combine(_directory, "full-state.db");
        var firstDatabase = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        var reservationStore = new SqliteCommandTransactionReservationStore(firstDatabase);
        var journal = new SqliteCommandTransactionJournal(firstDatabase);
        var equipmentStore = new SqliteEquipmentModelStore(firstDatabase);
        var b3Sink = new SqliteB3ArchiveSink(firstDatabase);
        var communicationSink = new SqliteCommunicationJournalSink(firstDatabase);
        var deviceId = new DeviceId(1001);

        await equipmentStore.AddInstallationAsync(
            new Installation(new InstallationId("MSM"), "Mont St Michel"));
        await equipmentStore.AddEquipmentAsync(
            new Equipment(new EquipmentId("CHILLER-1"), new InstallationId("MSM"), "Chiller 1"));
        await equipmentStore.AddMeasurementPointAsync(
            new MeasurementPoint(new MeasurementPointId("P-01"), new EquipmentId("CHILLER-1"), "Motor DE"));
        await equipmentStore.AssignAsync(deviceId, new MeasurementPointId("P-01"), Start);

        await b3Sink.AppendAsync(new B3ArchiveObservation(
            deviceId,
            CreateB3(calculationSequence: 77),
            Start.AddMinutes(1)));

        var endpoint = new TR2Endpoint(new SerialBus("rs485-a"), new ModbusAddress(17));
        await communicationSink.AppendAsync(new CommunicationFailureEvent(
            endpoint,
            deviceId,
            CommunicationOperation.Polling,
            Start.AddMinutes(2),
            "System.TimeoutException",
            "No response"));

        var firstCoordinator = new CommandCoordinator(deviceId, reservationStore, journal);
        var prepared = await firstCoordinator.PrepareAsync("START_ACQUISITION", Start.AddMinutes(3));
        Assert.Equal((ushort)1, prepared.TransactionId.Value);
        await firstCoordinator.MarkSubmittedAsync(Start.AddMinutes(4));

        SqliteConnection.ClearAllPools();

        var reopenedDatabase = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        var reopenedReservationStore = new SqliteCommandTransactionReservationStore(reopenedDatabase);
        var reopenedJournal = new SqliteCommandTransactionJournal(reopenedDatabase);
        var reopenedEquipmentStore = new SqliteEquipmentModelStore(reopenedDatabase);
        var recoveredCoordinator = new CommandCoordinator(deviceId, reopenedReservationStore, reopenedJournal);
        var recovery = new CommandCoordinatorRecoveryService(reopenedJournal);

        var recovered = await recovery.RestoreAsync(recoveredCoordinator);

        Assert.NotNull(recovered);
        Assert.Equal(CommandTransactionState.Ambiguous, recovered!.State);
        Assert.Equal((ushort)1, recovered.TransactionId.Value);
        Assert.Equal("START_ACQUISITION", recovered.RequestIdentity);
        Assert.Equal(recovered, recoveredCoordinator.ActiveTransaction);
        Assert.Equal((ushort)1, (await reopenedReservationStore.GetLastAllocatedAsync(deviceId))!.Value.Value);

        var assignments = await reopenedEquipmentStore.ReadAssignmentHistoryAsync();
        var assignment = Assert.Single(assignments);
        Assert.Equal(deviceId, assignment.DeviceId);
        Assert.Equal(new MeasurementPointId("P-01"), assignment.MeasurementPointId);
        Assert.True(assignment.IsActive);

        using (var connection = reopenedDatabase.OpenConnection())
        {
            Assert.Equal(1, ReadInt32(connection, "SELECT COUNT(*) FROM b3_archive WHERE device_id = 1001 AND calculation_sequence = 77;"));
            Assert.Equal(1, ReadInt32(connection, "SELECT COUNT(*) FROM communication_failure_journal WHERE device_id = 1001 AND operation = 'Polling';"));
        }

        await recoveredCoordinator.ResolveTerminalAsync(
            recovered.TransactionId,
            Start.AddMinutes(5));
        var second = await recoveredCoordinator.PrepareAsync(
            "STOP_ACQUISITION",
            Start.AddMinutes(6));

        Assert.Equal((ushort)2, second.TransactionId.Value);
    }

    [Fact]
    public async Task ReservationWithoutJournal_IsControlledPartialStateAndDoesNotInventActiveCommand()
    {
        var databasePath = Path.Combine(_directory, "reservation-only.db");
        var firstDatabase = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        var firstReservationStore = new SqliteCommandTransactionReservationStore(firstDatabase);
        var deviceId = new DeviceId(2002);

        await firstReservationStore.PersistAllocationAsync(deviceId, new TransactionId(41));

        SqliteConnection.ClearAllPools();

        var reopenedDatabase = new SqliteDatabase(new SqlitePersistenceOptions(databasePath));
        var reservationStore = new SqliteCommandTransactionReservationStore(reopenedDatabase);
        var journal = new SqliteCommandTransactionJournal(reopenedDatabase);
        var coordinator = new CommandCoordinator(deviceId, reservationStore, journal);
        var recovery = new CommandCoordinatorRecoveryService(journal);

        Assert.Null(await recovery.RestoreAsync(coordinator));
        Assert.Null(coordinator.ActiveTransaction);
        Assert.Equal((ushort)41, (await reservationStore.GetLastAllocatedAsync(deviceId))!.Value.Value);

        var next = await coordinator.PrepareAsync("SELFTEST", Start);
        Assert.Equal((ushort)42, next.TransactionId.Value);
    }

    public void Dispose()
    {
        SqliteConnection.ClearAllPools();
        if (Directory.Exists(_directory))
            Directory.Delete(_directory, recursive: true);
    }

    private static B3VibrationSupervision CreateB3(uint calculationSequence) =>
        new(
            StatusGlobal: 1,
            ValidityFlags: 2,
            AlarmFlags: 0,
            SeverityGlobal: 1,
            LastUpdateTr2Seconds: 100,
            ValueAgeMilliseconds: 20,
            CalculationSequence: calculationSequence,
            WindowDurationMilliseconds: 1000,
            ValidSampleCount: 500,
            RmsGlobalMg: 250,
            PeakGlobalMg: 400,
            RmsXMg: 101,
            RmsYMg: 102,
            RmsZMg: 103,
            PeakXMg: 201,
            PeakYMg: 202,
            PeakZMg: 203,
            DominantAxis: 2,
            ExceedGlobal: 0,
            ExceedX: 0,
            ExceedY: 0,
            ExceedZ: 0,
            AlarmLatched: 0,
            ExceedCount: 0,
            AlarmCount: 0);

    private static int ReadInt32(SqliteConnection connection, string sql)
    {
        using var command = connection.CreateCommand();
        command.CommandText = sql;
        return Convert.ToInt32(command.ExecuteScalar(), CultureInfo.InvariantCulture);
    }
}
