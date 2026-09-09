using NModbus;

namespace TR2.Transport;

public static class NModbusSafetyPolicy
{
    public static void Apply(IModbusMaster master)
    {
        ArgumentNullException.ThrowIfNull(master);

        master.Transport.Retries = 0;
        master.Transport.WaitToRetryMilliseconds = 0;
        master.Transport.SlaveBusyUsesRetryCount = true;
    }
}
