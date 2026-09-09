using TR2.Domain;
using Xunit;

namespace TR2.Application.Tests;

public sealed class EquipmentAssignmentRegistryTests
{
    private static readonly DateTimeOffset Start =
        new(2026, 9, 9, 8, 0, 0, TimeSpan.Zero);

    [Fact]
    public void A_device_can_have_zero_or_one_active_measurement_point()
    {
        var registry = new EquipmentAssignmentRegistry();
        var device = new DeviceId(1001);

        var assignment = registry.Assign(device, new MeasurementPointId("P-01"), Start);

        Assert.Equal(assignment, registry.GetActiveForDevice(device));
        Assert.Throws<InvalidOperationException>(() =>
            registry.Assign(device, new MeasurementPointId("P-02"), Start.AddMinutes(1)));
    }

    [Fact]
    public void A_measurement_point_can_have_at_most_one_active_device()
    {
        var registry = new EquipmentAssignmentRegistry();
        var point = new MeasurementPointId("P-01");
        registry.Assign(new DeviceId(1001), point, Start);

        Assert.Throws<InvalidOperationException>(() =>
            registry.Assign(new DeviceId(2002), point, Start.AddMinutes(1)));
    }

    [Fact]
    public void Moving_a_device_closes_previous_assignment_and_creates_new_history_entry()
    {
        var registry = new EquipmentAssignmentRegistry();
        var device = new DeviceId(1001);
        var firstPoint = new MeasurementPointId("P-01");
        var secondPoint = new MeasurementPointId("P-02");
        registry.Assign(device, firstPoint, Start);

        var movedAt = Start.AddHours(2);
        var replacement = registry.Move(device, secondPoint, movedAt);

        Assert.Equal(2, registry.History.Count);
        Assert.Equal(movedAt, registry.History[0].ValidTo);
        Assert.Equal(firstPoint, registry.History[0].MeasurementPointId);
        Assert.False(registry.History[0].IsActive);
        Assert.Equal(replacement, registry.History[1]);
        Assert.Equal(replacement, registry.GetActiveForDevice(device));
        Assert.Null(registry.GetActiveForPoint(firstPoint));
        Assert.Equal(replacement, registry.GetActiveForPoint(secondPoint));
    }

    [Fact]
    public void A_closed_point_can_be_assigned_to_another_device_without_rewriting_history()
    {
        var registry = new EquipmentAssignmentRegistry();
        var point = new MeasurementPointId("P-01");
        var firstDevice = new DeviceId(1001);
        registry.Assign(firstDevice, point, Start);
        registry.Unassign(firstDevice, Start.AddHours(1));

        var second = registry.Assign(new DeviceId(2002), point, Start.AddHours(2));

        Assert.Equal(2, registry.History.Count);
        Assert.Equal(new DeviceId(1001), registry.History[0].DeviceId);
        Assert.False(registry.History[0].IsActive);
        Assert.Equal(second, registry.History[1]);
    }

    [Fact]
    public void Move_rejects_destination_that_is_already_active()
    {
        var registry = new EquipmentAssignmentRegistry();
        registry.Assign(new DeviceId(1001), new MeasurementPointId("P-01"), Start);
        registry.Assign(new DeviceId(2002), new MeasurementPointId("P-02"), Start);

        Assert.Throws<InvalidOperationException>(() =>
            registry.Move(new DeviceId(1001), new MeasurementPointId("P-02"), Start.AddHours(1)));
    }

    [Fact]
    public void Move_and_unassign_reject_retroactive_closure()
    {
        var registry = new EquipmentAssignmentRegistry();
        var device = new DeviceId(1001);
        registry.Assign(device, new MeasurementPointId("P-01"), Start);

        Assert.Throws<ArgumentOutOfRangeException>(() =>
            registry.Move(device, new MeasurementPointId("P-02"), Start));

        Assert.Throws<ArgumentOutOfRangeException>(() =>
            registry.Unassign(device, Start.AddTicks(-1)));
    }

    [Fact]
    public void Moving_or_unassigning_unknown_device_is_rejected()
    {
        var registry = new EquipmentAssignmentRegistry();
        var device = new DeviceId(1001);

        Assert.Throws<InvalidOperationException>(() =>
            registry.Move(device, new MeasurementPointId("P-01"), Start));
        Assert.Throws<InvalidOperationException>(() =>
            registry.Unassign(device, Start));
    }
}
