using System.IO.Ports;
using NModbus;
using NModbus.Serial;

namespace TR2.Transport;

public sealed class NModbusSerialBusConnectionFactory : IModbusBusConnectionFactory
{
    public ValueTask<IModbusBusConnection> OpenAsync(
        string busId,
        ModbusSerialConnectionSettings settings,
        CancellationToken cancellationToken = default)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(busId);
        ArgumentNullException.ThrowIfNull(settings);
        cancellationToken.ThrowIfCancellationRequested();

        var serialPort = new SerialPort(
            settings.PortName,
            settings.BaudRate,
            MapParity(settings.Parity),
            settings.DataBits,
            MapStopBits(settings.StopBits))
        {
            ReadTimeout = settings.ResponseTimeoutMilliseconds,
            WriteTimeout = settings.ResponseTimeoutMilliseconds
        };

        try
        {
            serialPort.Open();
            cancellationToken.ThrowIfCancellationRequested();

            var modbusFactory = new ModbusFactory();
            var master = modbusFactory.CreateRtuMaster(serialPort);

            try
            {
                NModbusSafetyPolicy.Apply(master);
                master.Transport.ReadTimeout = settings.ResponseTimeoutMilliseconds;
                master.Transport.WriteTimeout = settings.ResponseTimeoutMilliseconds;

                var client = new NModbusRegisterClient(master);
                var registerTransport = new ModbusRegisterTransport(busId, client);
                IModbusBusConnection connection = new NModbusSerialBusConnection(
                    busId,
                    master,
                    registerTransport);

                return ValueTask.FromResult(connection);
            }
            catch
            {
                master.Dispose();
                throw;
            }
        }
        catch
        {
            serialPort.Dispose();
            throw;
        }
    }

    private static Parity MapParity(ModbusSerialParity parity) => parity switch
    {
        ModbusSerialParity.None => Parity.None,
        ModbusSerialParity.Odd => Parity.Odd,
        ModbusSerialParity.Even => Parity.Even,
        ModbusSerialParity.Mark => Parity.Mark,
        ModbusSerialParity.Space => Parity.Space,
        _ => throw new ArgumentOutOfRangeException(nameof(parity))
    };

    private static StopBits MapStopBits(ModbusSerialStopBits stopBits) => stopBits switch
    {
        ModbusSerialStopBits.One => StopBits.One,
        ModbusSerialStopBits.Two => StopBits.Two,
        ModbusSerialStopBits.OnePointFive => StopBits.OnePointFive,
        _ => throw new ArgumentOutOfRangeException(nameof(stopBits))
    };
}
