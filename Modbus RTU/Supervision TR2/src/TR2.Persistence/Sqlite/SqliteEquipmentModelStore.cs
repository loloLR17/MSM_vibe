using System.Globalization;
using Microsoft.Data.Sqlite;
using TR2.Domain;

namespace TR2.Persistence.Sqlite;

public sealed class SqliteEquipmentModelStore
{
    private readonly SqliteDatabase _database;

    public SqliteEquipmentModelStore(SqliteDatabase database)
    {
        _database = database ?? throw new ArgumentNullException(nameof(database));
    }

    public ValueTask AddInstallationAsync(
        Installation installation,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(installation);
        cancellationToken.ThrowIfCancellationRequested();

        using var connection = _database.OpenConnection();
        using var transaction = connection.BeginTransaction();
        using var command = connection.CreateCommand();
        command.Transaction = transaction;
        command.CommandText = """
            INSERT INTO installation(installation_id, name)
            VALUES ($installation_id, $name);
            """;
        command.Parameters.AddWithValue("$installation_id", installation.Id.Value);
        command.Parameters.AddWithValue("$name", installation.Name);
        command.ExecuteNonQuery();
        transaction.Commit();
        return ValueTask.CompletedTask;
    }

    public ValueTask AddEquipmentAsync(
        Equipment equipment,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(equipment);
        cancellationToken.ThrowIfCancellationRequested();

        using var connection = _database.OpenConnection();
        using var transaction = connection.BeginTransaction();
        using var command = connection.CreateCommand();
        command.Transaction = transaction;
        command.CommandText = """
            INSERT INTO equipment(equipment_id, installation_id, name)
            VALUES ($equipment_id, $installation_id, $name);
            """;
        command.Parameters.AddWithValue("$equipment_id", equipment.Id.Value);
        command.Parameters.AddWithValue("$installation_id", equipment.InstallationId.Value);
        command.Parameters.AddWithValue("$name", equipment.Name);
        command.ExecuteNonQuery();
        transaction.Commit();
        return ValueTask.CompletedTask;
    }

    public ValueTask AddMeasurementPointAsync(
        MeasurementPoint measurementPoint,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(measurementPoint);
        cancellationToken.ThrowIfCancellationRequested();

        using var connection = _database.OpenConnection();
        using var transaction = connection.BeginTransaction();
        using var command = connection.CreateCommand();
        command.Transaction = transaction;
        command.CommandText = """
            INSERT INTO measurement_point(measurement_point_id, equipment_id, name)
            VALUES ($measurement_point_id, $equipment_id, $name);
            """;
        command.Parameters.AddWithValue("$measurement_point_id", measurementPoint.Id.Value);
        command.Parameters.AddWithValue("$equipment_id", measurementPoint.EquipmentId.Value);
        command.Parameters.AddWithValue("$name", measurementPoint.Name);
        command.ExecuteNonQuery();
        transaction.Commit();
        return ValueTask.CompletedTask;
    }

    public ValueTask<IReadOnlyList<Installation>> ReadInstallationsAsync(
        CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();
        using var connection = _database.OpenConnection();
        using var command = connection.CreateCommand();
        command.CommandText = "SELECT installation_id, name FROM installation ORDER BY installation_id;";
        using var reader = command.ExecuteReader();

        var items = new List<Installation>();
        while (reader.Read())
            items.Add(new Installation(new InstallationId(reader.GetString(0)), reader.GetString(1)));
        return ValueTask.FromResult<IReadOnlyList<Installation>>(items);
    }

    public ValueTask<IReadOnlyList<Equipment>> ReadEquipmentAsync(
        CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();
        using var connection = _database.OpenConnection();
        using var command = connection.CreateCommand();
        command.CommandText = "SELECT equipment_id, installation_id, name FROM equipment ORDER BY equipment_id;";
        using var reader = command.ExecuteReader();

        var items = new List<Equipment>();
        while (reader.Read())
        {
            items.Add(new Equipment(
                new EquipmentId(reader.GetString(0)),
                new InstallationId(reader.GetString(1)),
                reader.GetString(2)));
        }
        return ValueTask.FromResult<IReadOnlyList<Equipment>>(items);
    }

    public ValueTask<IReadOnlyList<MeasurementPoint>> ReadMeasurementPointsAsync(
        CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();
        using var connection = _database.OpenConnection();
        using var command = connection.CreateCommand();
        command.CommandText = "SELECT measurement_point_id, equipment_id, name FROM measurement_point ORDER BY measurement_point_id;";
        using var reader = command.ExecuteReader();

        var items = new List<MeasurementPoint>();
        while (reader.Read())
        {
            items.Add(new MeasurementPoint(
                new MeasurementPointId(reader.GetString(0)),
                new EquipmentId(reader.GetString(1)),
                reader.GetString(2)));
        }
        return ValueTask.FromResult<IReadOnlyList<MeasurementPoint>>(items);
    }

    public ValueTask<EquipmentAssignment> AssignAsync(
        DeviceId deviceId,
        MeasurementPointId measurementPointId,
        DateTimeOffset validFrom,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(measurementPointId);
        cancellationToken.ThrowIfCancellationRequested();

        using var connection = _database.OpenConnection();
        using var transaction = connection.BeginTransaction();

        if (HasActiveDevice(connection, transaction, deviceId))
            throw new InvalidOperationException("The device already has an active measurement-point assignment.");
        if (HasActivePoint(connection, transaction, measurementPointId))
            throw new InvalidOperationException("The measurement point already has an active TR2 assignment.");

        var assignment = new EquipmentAssignment(deviceId, measurementPointId, validFrom);
        InsertAssignment(connection, transaction, assignment);
        transaction.Commit();
        return ValueTask.FromResult(assignment);
    }

    public ValueTask<EquipmentAssignment> MoveAsync(
        DeviceId deviceId,
        MeasurementPointId newMeasurementPointId,
        DateTimeOffset movedAt,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(newMeasurementPointId);
        cancellationToken.ThrowIfCancellationRequested();

        using var connection = _database.OpenConnection();
        using var transaction = connection.BeginTransaction();
        var current = ReadActiveForDevice(connection, transaction, deviceId);
        if (current is null)
            throw new InvalidOperationException("The device has no active assignment to move.");
        if (HasActivePoint(connection, transaction, newMeasurementPointId))
            throw new InvalidOperationException("The destination measurement point already has an active TR2 assignment.");

        var closed = current.Value.Assignment.Close(movedAt);
        CloseAssignment(connection, transaction, current.Value.AssignmentId, closed.ValidTo!.Value);

        var replacement = new EquipmentAssignment(deviceId, newMeasurementPointId, movedAt);
        InsertAssignment(connection, transaction, replacement);
        transaction.Commit();
        return ValueTask.FromResult(replacement);
    }

    public ValueTask<EquipmentAssignment> UnassignAsync(
        DeviceId deviceId,
        DateTimeOffset validTo,
        CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        using var connection = _database.OpenConnection();
        using var transaction = connection.BeginTransaction();
        var current = ReadActiveForDevice(connection, transaction, deviceId);
        if (current is null)
            throw new InvalidOperationException("The device has no active assignment to close.");

        var closed = current.Value.Assignment.Close(validTo);
        CloseAssignment(connection, transaction, current.Value.AssignmentId, closed.ValidTo!.Value);
        transaction.Commit();
        return ValueTask.FromResult(closed);
    }

    public ValueTask<IReadOnlyList<EquipmentAssignment>> ReadAssignmentHistoryAsync(
        CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();
        using var connection = _database.OpenConnection();
        using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT device_id, measurement_point_id, valid_from_utc, valid_to_utc
            FROM equipment_assignment
            ORDER BY assignment_id ASC;
            """;
        using var reader = command.ExecuteReader();

        var items = new List<EquipmentAssignment>();
        while (reader.Read())
        {
            items.Add(new EquipmentAssignment(
                new DeviceId(checked((uint)reader.GetInt64(0))),
                new MeasurementPointId(reader.GetString(1)),
                ParseUtc(reader.GetString(2)),
                reader.IsDBNull(3) ? null : ParseUtc(reader.GetString(3))));
        }
        return ValueTask.FromResult<IReadOnlyList<EquipmentAssignment>>(items);
    }

    private static bool HasActiveDevice(SqliteConnection connection, SqliteTransaction transaction, DeviceId deviceId)
    {
        using var command = connection.CreateCommand();
        command.Transaction = transaction;
        command.CommandText = "SELECT 1 FROM equipment_assignment WHERE device_id = $device_id AND valid_to_utc IS NULL LIMIT 1;";
        command.Parameters.AddWithValue("$device_id", (long)deviceId.Value);
        return command.ExecuteScalar() is not null;
    }

    private static bool HasActivePoint(SqliteConnection connection, SqliteTransaction transaction, MeasurementPointId pointId)
    {
        using var command = connection.CreateCommand();
        command.Transaction = transaction;
        command.CommandText = "SELECT 1 FROM equipment_assignment WHERE measurement_point_id = $point_id AND valid_to_utc IS NULL LIMIT 1;";
        command.Parameters.AddWithValue("$point_id", pointId.Value);
        return command.ExecuteScalar() is not null;
    }

    private static (long AssignmentId, EquipmentAssignment Assignment)? ReadActiveForDevice(
        SqliteConnection connection,
        SqliteTransaction transaction,
        DeviceId deviceId)
    {
        using var command = connection.CreateCommand();
        command.Transaction = transaction;
        command.CommandText = """
            SELECT assignment_id, measurement_point_id, valid_from_utc
            FROM equipment_assignment
            WHERE device_id = $device_id AND valid_to_utc IS NULL
            LIMIT 1;
            """;
        command.Parameters.AddWithValue("$device_id", (long)deviceId.Value);
        using var reader = command.ExecuteReader();
        if (!reader.Read())
            return null;

        return (
            reader.GetInt64(0),
            new EquipmentAssignment(
                deviceId,
                new MeasurementPointId(reader.GetString(1)),
                ParseUtc(reader.GetString(2))));
    }

    private static void InsertAssignment(
        SqliteConnection connection,
        SqliteTransaction transaction,
        EquipmentAssignment assignment)
    {
        using var command = connection.CreateCommand();
        command.Transaction = transaction;
        command.CommandText = """
            INSERT INTO equipment_assignment(device_id, measurement_point_id, valid_from_utc, valid_to_utc)
            VALUES ($device_id, $measurement_point_id, $valid_from_utc, NULL);
            """;
        command.Parameters.AddWithValue("$device_id", (long)assignment.DeviceId.Value);
        command.Parameters.AddWithValue("$measurement_point_id", assignment.MeasurementPointId.Value);
        command.Parameters.AddWithValue("$valid_from_utc", UtcText(assignment.ValidFrom));
        command.ExecuteNonQuery();
    }

    private static void CloseAssignment(
        SqliteConnection connection,
        SqliteTransaction transaction,
        long assignmentId,
        DateTimeOffset validTo)
    {
        using var command = connection.CreateCommand();
        command.Transaction = transaction;
        command.CommandText = "UPDATE equipment_assignment SET valid_to_utc = $valid_to_utc WHERE assignment_id = $assignment_id;";
        command.Parameters.AddWithValue("$valid_to_utc", UtcText(validTo));
        command.Parameters.AddWithValue("$assignment_id", assignmentId);
        if (command.ExecuteNonQuery() != 1)
            throw new InvalidOperationException("The active assignment could not be closed.");
    }

    private static string UtcText(DateTimeOffset value) =>
        value.ToUniversalTime().ToString("O", CultureInfo.InvariantCulture);

    private static DateTimeOffset ParseUtc(string value) =>
        DateTimeOffset.ParseExact(value, "O", CultureInfo.InvariantCulture, DateTimeStyles.RoundtripKind);
}
