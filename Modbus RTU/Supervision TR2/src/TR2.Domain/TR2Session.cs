namespace TR2.Domain;

public enum TR2SessionState
{
    Unidentified,
    Compatible,
    Incompatible,
    Disconnected
}

public sealed record TR2Session
{
    private TR2Session(TR2Endpoint endpoint, TR2Device? device, TR2SessionState state)
    {
        Endpoint = endpoint;
        Device = device;
        State = state;
    }

    public TR2Endpoint Endpoint { get; }

    public TR2Device? Device { get; }

    public TR2SessionState State { get; }

    public static TR2Session CreateUnidentified(TR2Endpoint endpoint)
    {
        ArgumentNullException.ThrowIfNull(endpoint);
        return new TR2Session(endpoint, null, TR2SessionState.Unidentified);
    }

    public static TR2Session CreateCompatible(TR2Endpoint endpoint, TR2Device device)
    {
        ArgumentNullException.ThrowIfNull(endpoint);
        ArgumentNullException.ThrowIfNull(device);
        return new TR2Session(endpoint, device, TR2SessionState.Compatible);
    }

    public static TR2Session CreateIncompatible(TR2Endpoint endpoint)
    {
        ArgumentNullException.ThrowIfNull(endpoint);
        return new TR2Session(endpoint, null, TR2SessionState.Incompatible);
    }

    public TR2Session MarkDisconnected()
    {
        return new TR2Session(Endpoint, Device, TR2SessionState.Disconnected);
    }
}
