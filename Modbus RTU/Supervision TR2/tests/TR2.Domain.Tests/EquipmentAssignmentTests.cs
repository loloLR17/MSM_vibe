using Xunit;

namespace TR2.Domain.Tests;

public sealed class EquipmentAssignmentTests
{
    [Theory]
    [InlineData(null)]
    [InlineData("")]
    [InlineData("   ")]
    public void Business_identifiers_reject_missing_values(string? value)
    {
        Assert.ThrowsAny<ArgumentException>(() => new InstallationId(value!));
        Assert.ThrowsAny<ArgumentException>(() => new EquipmentId(value!));
        Assert.ThrowsAny<ArgumentException>(() => new MeasurementPointId(value!));
    }

    [Fact]
    public void Hierarchy_is_installation_then_equipment_then_measurement_point()
    {
        var installationId = new InstallationId("SHIP-A");
        var equipmentId = new EquipmentId("ENG-01");
        var pointId = new MeasurementPointId("ENG-01-DE");

        var installation = new Installation(installationId, "Installation A");
        var equipment = new Equipment(equipmentId, installationId, "Main engine 1");
        var point = new MeasurementPoint(pointId, equipmentId, "Drive end bearing");

        Assert.Equal(installationId, installation.Id);
        Assert.Equal(installation.Id, equipment.InstallationId);
        Assert.Equal(equipment.Id, point.EquipmentId);
    }

    [Fact]
    public void Active_assignment_is_indexed_by_device_identity_and_measurement_point()
    {
        var validFrom = new DateTimeOffset(2026, 9, 9, 8, 0, 0, TimeSpan.Zero);
        var assignment = new EquipmentAssignment(
            new DeviceId(1001),
            new MeasurementPointId("ENG-01-DE"),
            validFrom);

        Assert.Equal(new DeviceId(1001), assignment.DeviceId);
        Assert.Equal(new MeasurementPointId("ENG-01-DE"), assignment.MeasurementPointId);
        Assert.Equal(validFrom, assignment.ValidFrom);
        Assert.Null(assignment.ValidTo);
        Assert.True(assignment.IsActive);
    }

    [Fact]
    public void Closing_assignment_preserves_historical_context()
    {
        var validFrom = new DateTimeOffset(2026, 9, 9, 8, 0, 0, TimeSpan.Zero);
        var validTo = validFrom.AddHours(2);
        var active = new EquipmentAssignment(
            new DeviceId(1001),
            new MeasurementPointId("ENG-01-DE"),
            validFrom);

        var closed = active.Close(validTo);

        Assert.Equal(active.DeviceId, closed.DeviceId);
        Assert.Equal(active.MeasurementPointId, closed.MeasurementPointId);
        Assert.Equal(validFrom, closed.ValidFrom);
        Assert.Equal(validTo, closed.ValidTo);
        Assert.False(closed.IsActive);
        Assert.True(active.IsActive);
    }

    [Fact]
    public void Assignment_rejects_non_positive_validity_interval()
    {
        var validFrom = new DateTimeOffset(2026, 9, 9, 8, 0, 0, TimeSpan.Zero);
        var pointId = new MeasurementPointId("ENG-01-DE");

        Assert.Throws<ArgumentOutOfRangeException>(() =>
            new EquipmentAssignment(new DeviceId(1001), pointId, validFrom, validFrom));

        Assert.Throws<ArgumentOutOfRangeException>(() =>
            new EquipmentAssignment(new DeviceId(1001), pointId, validFrom, validFrom.AddTicks(-1)));
    }
}
