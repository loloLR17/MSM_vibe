using TR2.Domain;
using TR2.Protocol;

namespace TR2.Application;

public sealed record DeviceTelemetrySnapshots(
    DeviceId DeviceId,
    ObservedSnapshot<B1SystemState> SystemState,
    ObservedSnapshot<B2TimeState> TimeState,
    ObservedSnapshot<B3VibrationSupervision> VibrationState,
    ObservedSnapshot<B4ConfigurationState> ConfigurationState,
    ObservedSnapshot<B7DiagnosticState> DiagnosticState)
{
    public static DeviceTelemetrySnapshots Empty(DeviceId deviceId) =>
        new(
            deviceId,
            ObservedSnapshot<B1SystemState>.NeverReceived(),
            ObservedSnapshot<B2TimeState>.NeverReceived(),
            ObservedSnapshot<B3VibrationSupervision>.NeverReceived(),
            ObservedSnapshot<B4ConfigurationState>.NeverReceived(),
            ObservedSnapshot<B7DiagnosticState>.NeverReceived());

    public DeviceTelemetrySnapshots ReceiveSystemState(B1SystemState value, DateTimeOffset receivedAt) =>
        this with { SystemState = SystemState.Receive(value, receivedAt) };

    public DeviceTelemetrySnapshots ReceiveTimeState(B2TimeState value, DateTimeOffset receivedAt) =>
        this with { TimeState = TimeState.Receive(value, receivedAt) };

    public DeviceTelemetrySnapshots ReceiveVibrationState(B3VibrationSupervision value, DateTimeOffset receivedAt) =>
        this with { VibrationState = VibrationState.Receive(value, receivedAt) };

    public DeviceTelemetrySnapshots ReceiveConfigurationState(B4ConfigurationState value, DateTimeOffset receivedAt) =>
        this with { ConfigurationState = ConfigurationState.Receive(value, receivedAt) };

    public DeviceTelemetrySnapshots ReceiveDiagnosticState(B7DiagnosticState value, DateTimeOffset receivedAt) =>
        this with { DiagnosticState = DiagnosticState.Receive(value, receivedAt) };

    public DeviceTelemetrySnapshots MarkUnavailable() =>
        this with
        {
            SystemState = SystemState.MarkUnavailable(),
            TimeState = TimeState.MarkUnavailable(),
            VibrationState = VibrationState.MarkUnavailable(),
            ConfigurationState = ConfigurationState.MarkUnavailable(),
            DiagnosticState = DiagnosticState.MarkUnavailable()
        };
}
