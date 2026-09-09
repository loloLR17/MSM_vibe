using TR2.Transport;
using Xunit;

namespace TR2.Transport.Tests;

public sealed class ModbusSerialConnectionSettingsTests
{
    [Fact]
    public void ConstructorPreservesExplicitSettings()
    {
        var settings = new ModbusSerialConnectionSettings(
            "COM7",
            115200,
            8,
            ModbusSerialParity.Even,
            ModbusSerialStopBits.Two,
            750);

        Assert.Equal("COM7", settings.PortName);
        Assert.Equal(115200, settings.BaudRate);
        Assert.Equal(8, settings.DataBits);
        Assert.Equal(ModbusSerialParity.Even, settings.Parity);
        Assert.Equal(ModbusSerialStopBits.Two, settings.StopBits);
        Assert.Equal(750, settings.ResponseTimeoutMilliseconds);
    }

    [Theory]
    [InlineData(0, 8, 750)]
    [InlineData(-1, 8, 750)]
    [InlineData(115200, 0, 750)]
    [InlineData(115200, -1, 750)]
    [InlineData(115200, 8, 0)]
    [InlineData(115200, 8, -1)]
    public void ConstructorRejectsNonPositiveNumericSettings(
        int baudRate,
        int dataBits,
        int responseTimeoutMilliseconds)
    {
        Assert.Throws<ArgumentOutOfRangeException>(() =>
            new ModbusSerialConnectionSettings(
                "COM7",
                baudRate,
                dataBits,
                ModbusSerialParity.None,
                ModbusSerialStopBits.One,
                responseTimeoutMilliseconds));
    }

    [Fact]
    public void ConstructorRejectsBlankPortName()
    {
        Assert.Throws<ArgumentException>(() =>
            new ModbusSerialConnectionSettings(
                " ",
                115200,
                8,
                ModbusSerialParity.None,
                ModbusSerialStopBits.One,
                750));
    }
}
