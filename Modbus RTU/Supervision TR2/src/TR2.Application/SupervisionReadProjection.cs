using TR2.Domain;
using TR2.Protocol;

namespace TR2.Application;

public sealed class SupervisionReadProjection
{
    private readonly FleetRegistry _fleetRegistry;
    private readonly DeviceTelemetrySnapshotRegistry _telemetryRegistry;
    private readonly SnapshotFreshnessPolicy _freshnessPolicy;

    public SupervisionReadProjection(
        FleetRegistry fleetRegistry,
        DeviceTelemetrySnapshotRegistry telemetryRegistry,
        SnapshotFreshnessPolicy freshnessPolicy)
    {
        ArgumentNullException.ThrowIfNull(fleetRegistry);
        ArgumentNullException.ThrowIfNull(telemetryRegistry);
        ArgumentNullException.ThrowIfNull(freshnessPolicy);

        _fleetRegistry = fleetRegistry;
        _telemetryRegistry = telemetryRegistry;
        _freshnessPolicy = freshnessPolicy;
    }

    public IReadOnlyList<IhmDeviceReadModel> GetFleet(DateTimeOffset observedAt) =>
        _fleetRegistry.Sessions
            .OrderBy(session => session.Endpoint.Bus.Id, StringComparer.Ordinal)
            .ThenBy(session => session.Endpoint.Address.Value)
            .Select(session => ProjectSession(session, observedAt))
            .ToArray();

    public IhmDeviceReadModel? GetDevice(uint deviceId, DateTimeOffset observedAt) =>
        GetFleet(observedAt).SingleOrDefault(device => device.DeviceId == deviceId);

    private IhmDeviceReadModel ProjectSession(TR2Session session, DateTimeOffset observedAt)
    {
        var deviceId = session.Device?.DeviceId;
        var telemetry = deviceId is null
            ? null
            : ProjectTelemetry(_telemetryRegistry.Get(deviceId.Value), observedAt);

        return new IhmDeviceReadModel(
            session.Endpoint.Bus.Id,
            session.Endpoint.Address.Value,
            deviceId?.Value,
            ProjectSessionState(session.State),
            telemetry);
    }

    private IhmDeviceTelemetryReadModel ProjectTelemetry(
        DeviceTelemetrySnapshots snapshots,
        DateTimeOffset observedAt) =>
        new(
            ProjectObserved(
                snapshots.SystemState,
                observedAt,
                value => new IhmSystemStateReadModel(
                    value.SystemStatus,
                    value.SystemFlags,
                    value.FaultFlags,
                    value.WarningFlags,
                    value.UptimeSeconds,
                    value.LastResetCause,
                    value.InternalTemperatureDeciCelsius,
                    value.CpuLoadPercent,
                    value.MemoryUsagePercent,
                    value.StorageStatus,
                    value.StorageUsagePercent,
                    value.AcquisitionState,
                    value.ActiveCampaignId,
                    value.ErrorCode,
                    value.WarningCode)),
            ProjectObserved(
                snapshots.TimeState,
                observedAt,
                value => new IhmTimeStateReadModel(
                    value.TimeStatus,
                    value.TimeFlags,
                    value.CurrentTimeSeconds,
                    value.LastSyncTimeSeconds,
                    value.TimeSinceSyncSeconds,
                    value.PreparedTimeSeconds,
                    value.PreparedTimeStatus,
                    value.TimeAccuracyMilliseconds,
                    value.DriftPpm,
                    value.SyncSource)),
            ProjectObserved(
                snapshots.VibrationState,
                observedAt,
                value => new IhmVibrationStateReadModel(
                    value.StatusGlobal,
                    value.ValidityFlags,
                    value.AlarmFlags,
                    value.SeverityGlobal,
                    value.LastUpdateTr2Seconds,
                    value.ValueAgeMilliseconds,
                    value.CalculationSequence,
                    value.WindowDurationMilliseconds,
                    value.ValidSampleCount,
                    value.RmsGlobalMg,
                    value.PeakGlobalMg,
                    value.RmsXMg,
                    value.RmsYMg,
                    value.RmsZMg,
                    value.PeakXMg,
                    value.PeakYMg,
                    value.PeakZMg,
                    value.DominantAxis,
                    value.ExceedGlobal,
                    value.ExceedX,
                    value.ExceedY,
                    value.ExceedZ,
                    value.AlarmLatched,
                    value.ExceedCount,
                    value.AlarmCount)),
            ProjectObserved(
                snapshots.DiagnosticState,
                observedAt,
                value => new IhmDiagnosticStateReadModel(
                    value.DiagnosticStructureVersion,
                    value.SystemHealthStatus,
                    value.SystemFaultFlags,
                    value.LastFaultCode,
                    value.LastFaultTimestampSeconds,
                    value.SelftestStatus,
                    value.SelftestResultCode,
                    value.SelftestDetail,
                    value.UptimeSeconds,
                    value.ResetCause,
                    value.InternalTemperatureDeciCelsius,
                    value.SupplyVoltageMillivolts)));

    private IhmObservedValue<TTarget> ProjectObserved<TSource, TTarget>(
        ObservedSnapshot<TSource> source,
        DateTimeOffset observedAt,
        Func<TSource, TTarget> map)
    {
        var value = source.HasValue && source.LastValue is not null
            ? map(source.LastValue)
            : default;

        return new IhmObservedValue<TTarget>(
            source.HasValue,
            source.HasValue && source.IsAvailable,
            source.ReceivedAt,
            ProjectFreshness(source.GetFreshness(_freshnessPolicy, observedAt)),
            value);
    }

    private static IhmSessionState ProjectSessionState(TR2SessionState state) =>
        state switch
        {
            TR2SessionState.Unidentified => IhmSessionState.Unidentified,
            TR2SessionState.Compatible => IhmSessionState.Compatible,
            TR2SessionState.Incompatible => IhmSessionState.Incompatible,
            TR2SessionState.Disconnected => IhmSessionState.Disconnected,
            _ => throw new ArgumentOutOfRangeException(nameof(state), state, null)
        };

    private static IhmSnapshotFreshness ProjectFreshness(SnapshotFreshness freshness) =>
        freshness switch
        {
            SnapshotFreshness.NeverReceived => IhmSnapshotFreshness.NeverReceived,
            SnapshotFreshness.Fresh => IhmSnapshotFreshness.Fresh,
            SnapshotFreshness.Aging => IhmSnapshotFreshness.Aging,
            SnapshotFreshness.Stale => IhmSnapshotFreshness.Stale,
            SnapshotFreshness.Unavailable => IhmSnapshotFreshness.Unavailable,
            _ => throw new ArgumentOutOfRangeException(nameof(freshness), freshness, null)
        };
}
