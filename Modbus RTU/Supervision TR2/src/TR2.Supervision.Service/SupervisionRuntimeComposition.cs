using TR2.Application;
using TR2.Persistence.Sqlite;
using TR2.Transport;

namespace TR2.Supervision.Service;

public sealed class SupervisionRuntimeComposition
{
    internal SupervisionRuntimeComposition(
        RuntimeConfiguration configuration,
        SqliteDatabase database,
        SqliteCommandTransactionReservationStore commandReservationStore,
        SqliteCommandTransactionJournal commandJournal,
        SqliteB3ArchiveSink b3ArchiveSink,
        SqliteCommunicationJournalSink communicationJournalSink,
        SqliteEquipmentModelStore equipmentModelStore,
        FleetRegistry fleetRegistry,
        CommandCoordinatorRegistry commandCoordinatorRegistry,
        DeviceTelemetrySnapshotRegistry telemetrySnapshotRegistry,
        BusWorkScheduler busWorkScheduler,
        RuntimeReadinessGate readinessGate,
        ModbusBusConnectionManager busConnectionManager)
    {
        Configuration = configuration;
        Database = database;
        CommandReservationStore = commandReservationStore;
        CommandJournal = commandJournal;
        B3ArchiveSink = b3ArchiveSink;
        CommunicationJournalSink = communicationJournalSink;
        EquipmentModelStore = equipmentModelStore;
        FleetRegistry = fleetRegistry;
        CommandCoordinatorRegistry = commandCoordinatorRegistry;
        TelemetrySnapshotRegistry = telemetrySnapshotRegistry;
        BusWorkScheduler = busWorkScheduler;
        ReadinessGate = readinessGate;
        BusConnectionManager = busConnectionManager;
    }

    public RuntimeConfiguration Configuration { get; }
    public SqliteDatabase Database { get; }
    public SqliteCommandTransactionReservationStore CommandReservationStore { get; }
    public SqliteCommandTransactionJournal CommandJournal { get; }
    public SqliteB3ArchiveSink B3ArchiveSink { get; }
    public SqliteCommunicationJournalSink CommunicationJournalSink { get; }
    public SqliteEquipmentModelStore EquipmentModelStore { get; }
    public FleetRegistry FleetRegistry { get; }
    public CommandCoordinatorRegistry CommandCoordinatorRegistry { get; }
    public DeviceTelemetrySnapshotRegistry TelemetrySnapshotRegistry { get; }
    public BusWorkScheduler BusWorkScheduler { get; }
    public RuntimeReadinessGate ReadinessGate { get; }
    public ModbusBusConnectionManager BusConnectionManager { get; }
}

public static class SupervisionRuntimeCompositionRoot
{
    public static SupervisionRuntimeComposition Compose(
        RuntimeConfiguration configuration,
        IModbusBusConnectionFactory? busConnectionFactory = null)
    {
        ArgumentNullException.ThrowIfNull(configuration);

        var database = new SqliteDatabase(configuration.Persistence);
        var commandReservationStore = new SqliteCommandTransactionReservationStore(database);
        var commandJournal = new SqliteCommandTransactionJournal(database);
        var b3ArchiveSink = new SqliteB3ArchiveSink(database);
        var communicationJournalSink = new SqliteCommunicationJournalSink(database);
        var equipmentModelStore = new SqliteEquipmentModelStore(database);

        var fleetRegistry = new FleetRegistry();
        foreach (var bus in configuration.Buses)
        {
            foreach (var endpoint in bus.Endpoints)
            {
                fleetRegistry.RegisterEndpoint(endpoint);
            }
        }

        var connectionManager = new ModbusBusConnectionManager(
            busConnectionFactory ?? new NModbusSerialBusConnectionFactory());

        return new SupervisionRuntimeComposition(
            configuration,
            database,
            commandReservationStore,
            commandJournal,
            b3ArchiveSink,
            communicationJournalSink,
            equipmentModelStore,
            fleetRegistry,
            new CommandCoordinatorRegistry(),
            new DeviceTelemetrySnapshotRegistry(),
            new BusWorkScheduler(),
            new RuntimeReadinessGate(),
            connectionManager);
    }
}
