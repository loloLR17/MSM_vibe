namespace TR2.Domain;

public sealed record TR2Endpoint
{
    public TR2Endpoint(SerialBus bus, ModbusAddress address)
    {
        ArgumentNullException.ThrowIfNull(bus);
        Bus = bus;
        Address = address;
    }

    public SerialBus Bus { get; }

    public ModbusAddress Address { get; }
}
