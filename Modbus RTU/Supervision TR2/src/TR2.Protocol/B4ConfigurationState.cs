namespace TR2.Protocol;

public sealed record B4ConfigurationState(
    ushort StructureVersion,
    ushort CapabilitiesMask,
    uint PreparedConfigId,
    uint ActiveConfigId,
    ushort ConfigState,
    ushort ConfigErrorCode,
    uint PreparedConfigCrc,
    uint ActiveConfigCrc,
    uint RevisionCounter,
    ushort SamplingFrequencyHz,
    ushort AxesEnableMask,
    ushort FullScaleCode,
    ushort AcquisitionMode,
    ushort WindowSizeSamples,
    ushort IndicatorPeriodMilliseconds,
    uint CampaignDurationSeconds,
    ushort StorageMode,
    uint StorageLimitMb,
    ushort SupervisionEnableMask,
    ushort RmsWarnThresholdMg,
    ushort RmsAlarmThresholdMg,
    ushort PeakWarnThresholdMg,
    ushort PeakAlarmThresholdMg,
    ushort ThresholdHysteresisMg,
    ushort AlarmHoldTimeMilliseconds,
    uint CampaignContextId,
    uint MissionId,
    string CampaignLabel,
    string MissionLabel,
    ushort OperatingModeCode,
    ushort NavigationZoneCode,
    ushort LoadStateCode,
    ushort SeaStateCode,
    ushort ActiveSamplingFrequencyHz,
    ushort ActiveAxesEnableMask,
    ushort ActiveFullScaleCode,
    ushort ActiveAcquisitionMode,
    ushort ActiveWindowSizeSamples,
    ushort ActiveIndicatorPeriodMilliseconds,
    uint ActiveCampaignDurationSeconds,
    ushort ActiveStorageMode,
    uint ActiveStorageLimitMb,
    ushort ActiveSupervisionEnableMask,
    ushort ActiveRmsWarnThresholdMg,
    ushort ActiveRmsAlarmThresholdMg,
    ushort ActivePeakWarnThresholdMg,
    ushort ActivePeakAlarmThresholdMg,
    ushort ActiveThresholdHysteresisMg,
    ushort ActiveAlarmHoldTimeMilliseconds,
    uint ActiveCampaignContextId,
    uint ActiveMissionId,
    string ActiveCampaignLabel,
    string ActiveMissionLabel,
    ushort ActiveOperatingModeCode,
    ushort ActiveNavigationZoneCode,
    ushort ActiveLoadStateCode,
    ushort ActiveSeaStateCode)
{
    public const ushort StartAddress = 4000;
    public const ushort RegisterCount = 176;

    public static B4ConfigurationState Parse(ReadOnlySpan<ushort> registers)
    {
        if (registers.Length < RegisterCount)
        {
            throw new ArgumentException(
                $"B4 requires at least {RegisterCount} registers.",
                nameof(registers));
        }

        return new B4ConfigurationState(
            registers[0],
            registers[1],
            UInt32(registers[2], registers[3]),
            UInt32(registers[4], registers[5]),
            registers[6],
            registers[7],
            UInt32(registers[8], registers[9]),
            UInt32(registers[10], registers[11]),
            UInt32(registers[12], registers[13]),
            registers[16],
            registers[18],
            registers[19],
            registers[20],
            registers[21],
            registers[22],
            UInt32(registers[23], registers[24]),
            registers[25],
            UInt32(registers[26], registers[27]),
            registers[40],
            registers[41],
            registers[42],
            registers[43],
            registers[44],
            registers[45],
            registers[46],
            UInt32(registers[56], registers[57]),
            UInt32(registers[58], registers[59]),
            FixedAscii(registers.Slice(60, 16)),
            FixedAscii(registers.Slice(76, 16)),
            registers[92],
            registers[93],
            registers[94],
            registers[95],
            registers[100],
            registers[101],
            registers[102],
            registers[103],
            registers[104],
            registers[105],
            UInt32(registers[106], registers[107]),
            registers[108],
            UInt32(registers[109], registers[110]),
            registers[116],
            registers[117],
            registers[118],
            registers[119],
            registers[120],
            registers[121],
            registers[122],
            UInt32(registers[128], registers[129]),
            UInt32(registers[130], registers[131]),
            FixedAscii(registers.Slice(132, 16)),
            FixedAscii(registers.Slice(148, 16)),
            registers[164],
            registers[165],
            registers[166],
            registers[167]);
    }

    private static uint UInt32(ushort msw, ushort lsw) =>
        ((uint)msw << 16) | lsw;

    private static string FixedAscii(ReadOnlySpan<ushort> registers)
    {
        Span<char> characters = stackalloc char[registers.Length * 2];
        var length = 0;

        foreach (var register in registers)
        {
            var high = (byte)(register >> 8);
            var low = (byte)(register & 0xFF);

            if (high == 0)
            {
                break;
            }

            characters[length++] = (char)high;

            if (low == 0)
            {
                break;
            }

            characters[length++] = (char)low;
        }

        return new string(characters[..length]);
    }
}
