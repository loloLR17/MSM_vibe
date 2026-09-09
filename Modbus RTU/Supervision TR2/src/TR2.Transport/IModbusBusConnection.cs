namespace TR2.Transport;

public interface IModbusBusConnection : IAsyncDisposable
{
    string BusId { get; }

    IRegisterTransport RegisterTransport { get; }

    IRegisterWriteTransport RegisterWriteTransport { get; }
}
