using TR2.Domain;

namespace TR2.Protocol;

public static class B0SessionFactory
{
    public static TR2Session Create(
        TR2Endpoint endpoint,
        ReadOnlySpan<ushort> registers,
        ushort supportedProtocolVersion)
    {
        ArgumentNullException.ThrowIfNull(endpoint);

        var identification = B0Identification.Parse(registers);
        if (!identification.SupportsProtocolVersion(supportedProtocolVersion))
        {
            return TR2Session.CreateIncompatible(endpoint);
        }

        return TR2Session.CreateCompatible(
            endpoint,
            new TR2Device(identification.DeviceId));
    }
}
