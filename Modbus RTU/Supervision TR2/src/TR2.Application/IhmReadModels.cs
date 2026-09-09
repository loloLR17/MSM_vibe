namespace TR2.Application;

public enum IhmSessionState { Unidentified, Compatible, Incompatible, Disconnected }
public enum IhmSnapshotFreshness { NeverReceived, Fresh, Aging, Stale, Unavailable }

public sealed record IhmObservedValue<T>(bool HasValue, bool IsAvailable, DateTimeOffset? ReceivedAt, IhmSnapshotFreshness Freshness, T? Value);

public sealed record IhmSystemStateReadModel(ushort SystemStatus, ushort SystemFlags, ushort FaultFlags, ushort WarningFlags, uint UptimeSeconds, ushort LastResetCause, short InternalTemperatureDeciCelsius, ushort CpuLoadPercent, ushort MemoryUsagePercent, ushort StorageStatus, ushort StorageUsagePercent, ushort AcquisitionState, uint ActiveCampaignId, ushort ErrorCode, ushort WarningCode);
public sealed record IhmTimeStateReadModel(ushort TimeStatus, ushort TimeFlags, uint CurrentTimeSeconds, uint LastSyncTimeSeconds, uint TimeSinceSyncSeconds, uint PreparedTimeSeconds, ushort PreparedTimeStatus, ushort TimeAccuracyMilliseconds, short DriftPpm, ushort SyncSource);
public sealed record IhmVibrationStateReadModel(ushort StatusGlobal, ushort ValidityFlags, ushort AlarmFlags, ushort SeverityGlobal, uint LastUpdateTr2Seconds, uint ValueAgeMilliseconds, uint CalculationSequence, uint WindowDurationMilliseconds, uint ValidSampleCount, uint RmsGlobalMg, uint PeakGlobalMg, uint RmsXMg, uint RmsYMg, uint RmsZMg, uint PeakXMg, uint PeakYMg, uint PeakZMg, ushort DominantAxis, ushort ExceedGlobal, ushort ExceedX, ushort ExceedY, ushort ExceedZ, ushort AlarmLatched, uint ExceedCount, uint AlarmCount);
public sealed record IhmDiagnosticStateReadModel(ushort DiagnosticStructureVersion, ushort SystemHealthStatus, ushort SystemFaultFlags, ushort LastFaultCode, uint LastFaultTimestampSeconds, ushort SelftestStatus, ushort SelftestResultCode, ushort SelftestDetail, uint UptimeSeconds, ushort ResetCause, short InternalTemperatureDeciCelsius, ushort SupplyVoltageMillivolts);

public sealed record IhmDeviceTelemetryReadModel(
    IhmObservedValue<IhmSystemStateReadModel> SystemState,
    IhmObservedValue<IhmTimeStateReadModel> TimeState,
    IhmObservedValue<IhmVibrationStateReadModel> VibrationState,
    IhmObservedValue<IhmConfigurationStateReadModel> ConfigurationState,
    IhmObservedValue<IhmCampaignInventoryReadModel> CampaignInventoryState,
    IhmObservedValue<IhmDiagnosticStateReadModel> DiagnosticState);

public sealed record IhmDeviceReadModel(string BusId, byte ModbusAddress, uint? DeviceId, IhmSessionState SessionState, IhmDeviceTelemetryReadModel? Telemetry);
