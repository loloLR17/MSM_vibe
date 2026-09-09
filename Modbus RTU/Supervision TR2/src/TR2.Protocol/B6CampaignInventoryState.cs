using System.Text;

namespace TR2.Protocol;

public sealed record B6CampaignInventoryState(
    ushort InventoryStructureVersion,
    ushort TotalCampaignCount,
    ushort ValidCampaignCount,
    ushort SelectedCampaignIndex,
    ushort SelectedCampaignValid,
    uint StorageUsedMb,
    uint StorageFreeMb,
    ushort StorageHealthStatus,
    uint CampaignId,
    uint MissionId,
    uint StartTimestampSeconds,
    uint EndTimestampSeconds,
    ushort CampaignState,
    uint DurationSeconds,
    uint DataSizeMb,
    string CampaignLabel,
    string MissionLabel,
    ushort DataIntegrityStatus)
{
    public const ushort StartAddress = 6000;
    public const ushort RegisterCount = 64;

    public static B6CampaignInventoryState Parse(ReadOnlySpan<ushort> registers)
    {
        if (registers.Length < RegisterCount)
        {
            throw new ArgumentException(
                $"B6 requires at least {RegisterCount} registers.",
                nameof(registers));
        }

        return new B6CampaignInventoryState(
            registers[0],
            registers[1],
            registers[2],
            registers[3],
            registers[4],
            UInt32(registers[5], registers[6]),
            UInt32(registers[7], registers[8]),
            registers[9],
            UInt32(registers[12], registers[13]),
            UInt32(registers[14], registers[15]),
            UInt32(registers[16], registers[17]),
            UInt32(registers[18], registers[19]),
            registers[20],
            UInt32(registers[21], registers[22]),
            UInt32(registers[23], registers[24]),
            DecodeFixedAscii(registers.Slice(25, 16)),
            DecodeFixedAscii(registers.Slice(41, 16)),
            registers[57]);
    }

    private static uint UInt32(ushort msw, ushort lsw) =>
        ((uint)msw << 16) | lsw;

    private static string DecodeFixedAscii(ReadOnlySpan<ushort> registers)
    {
        var builder = new StringBuilder(registers.Length * 2);

        foreach (var register in registers)
        {
            var high = (byte)(register >> 8);
            var low = (byte)(register & 0xFF);

            if (high == 0)
            {
                break;
            }

            builder.Append((char)high);

            if (low == 0)
            {
                break;
            }

            builder.Append((char)low);
        }

        return builder.ToString();
    }
}
