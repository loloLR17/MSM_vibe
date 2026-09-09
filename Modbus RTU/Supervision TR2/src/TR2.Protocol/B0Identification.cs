using TR2.Domain;

namespace TR2.Protocol;

public sealed record B0Identification(
    DeviceId DeviceId,
    ushort ProtocolVersion)
{
    public const int RegisterCount = 21;
    public const int DeviceIdMswOffset = 0;
    public const int DeviceIdLswOffset = 1;
    public const int ProtocolVersionOffset = 6;

    public static B0Identification Parse(ReadOnlySpan<ushort> registers)
    {
        if (registers.Length < RegisterCount)
        {
            throw new ArgumentException(
                $"B0 requires at least {RegisterCount} registers.",
                nameof(registers));
        }

        var deviceId = ((uint)registers[DeviceIdMswOffset] << 16)
            | registers[DeviceIdLswOffset];

        return new B0Identification(
            new DeviceId(deviceId),
            registers[ProtocolVersionOffset]);
    }

    public bool SupportsProtocolVersion(ushort supportedProtocolVersion) =>
        ProtocolVersion == supportedProtocolVersion;
}
