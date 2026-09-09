using TR2.Domain;
using TR2.Protocol;

namespace TR2.Application;

public sealed record PollingReadSet(
    PollingGroup Group,
    TR2Session? B0Session,
    B1SystemState? B1,
    B2TimeState? B2,
    B3VibrationSupervision? B3,
    B4ConfigurationState? B4,
    B5CommandState? B5,
    B6CampaignInventory? B6,
    B7DiagnosticState? B7);
