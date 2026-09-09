using Xunit;
using TR2.Supervision.Service;

namespace TR2.Supervision.Service.Tests;

public sealed class RuntimeConfigurationLoaderTests
{
    [Fact]
    public void ParseLoadsValidConfigurationAndResolvesRelativeDatabasePath()
    {
        var baseDirectory = Path.Combine(Path.GetTempPath(), "tr2-s3a-config");
        var configuration = RuntimeConfigurationLoader.Parse(
            """
            {
              "persistence": {
                "databasePath": "data/tr2-supervision.db"
              },
              "buses": [
                {
                  "id": "bus-1",
                  "serial": {
                    "portName": "COM7",
                    "baudRate": 115200,
                    "dataBits": 8,
                    "parity": "None",
                    "stopBits": "One",
                    "responseTimeoutMilliseconds": 750
                  },
                  "endpoints": [1, 2, 3]
                },
                {
                  "id": "bus-2",
                  "endpoints": []
                }
              ]
            }
            """,
            baseDirectory);

        Assert.Equal(
            Path.GetFullPath(Path.Combine(baseDirectory, "data/tr2-supervision.db")),
            configuration.Persistence.DatabasePath);
        Assert.Equal(2, configuration.Buses.Count);
        Assert.Equal("bus-1", configuration.Buses[0].Bus.Id);

        var serial = Assert.IsType<RuntimeSerialPortConfiguration>(configuration.Buses[0].Serial);
        Assert.Equal("COM7", serial.PortName);
        Assert.Equal(115200, serial.BaudRate);
        Assert.Equal(8, serial.DataBits);
        Assert.Equal(RuntimeSerialParity.None, serial.Parity);
        Assert.Equal(RuntimeSerialStopBits.One, serial.StopBits);
        Assert.Equal(TimeSpan.FromMilliseconds(750), serial.ResponseTimeout);

        Assert.Equal(new byte[] { 1, 2, 3 }, configuration.Buses[0].Endpoints.Select(endpoint => endpoint.Address.Value));
        Assert.Null(configuration.Buses[1].Serial);
        Assert.Empty(configuration.Buses[1].Endpoints);
    }

    [Fact]
    public void ParseRejectsIncompleteSerialConfiguration()
    {
        var exception = Assert.Throws<InvalidDataException>(() => RuntimeConfigurationLoader.Parse(
            """
            {
              "persistence": { "databasePath": "tr2.db" },
              "buses": [
                {
                  "id": "bus-1",
                  "serial": { "portName": "COM7" },
                  "endpoints": []
                }
              ]
            }
            """,
            Path.GetTempPath()));

        Assert.Contains("serial.baudRate", exception.Message);
    }

    [Theory]
    [InlineData("invalid", "One", "serial.parity")]
    [InlineData("None", "invalid", "serial.stopBits")]
    public void ParseRejectsUnsupportedSerialEnumValue(string parity, string stopBits, string expectedMessage)
    {
        var json = $$"""
            {
              "persistence": { "databasePath": "tr2.db" },
              "buses": [
                {
                  "id": "bus-1",
                  "serial": {
                    "portName": "COM7",
                    "baudRate": 115200,
                    "dataBits": 8,
                    "parity": "{{parity}}",
                    "stopBits": "{{stopBits}}",
                    "responseTimeoutMilliseconds": 750
                  },
                  "endpoints": []
                }
              ]
            }
            """;

        var exception = Assert.Throws<InvalidDataException>(() => RuntimeConfigurationLoader.Parse(json, Path.GetTempPath()));
        Assert.Contains(expectedMessage, exception.Message);
    }

    [Theory]
    [InlineData(-1)]
    [InlineData(0)]
    public void ParseRejectsNonPositiveSerialTimeout(int timeoutMilliseconds)
    {
        var json = $$"""
            {
              "persistence": { "databasePath": "tr2.db" },
              "buses": [
                {
                  "id": "bus-1",
                  "serial": {
                    "portName": "COM7",
                    "baudRate": 115200,
                    "dataBits": 8,
                    "parity": "None",
                    "stopBits": "One",
                    "responseTimeoutMilliseconds": {{timeoutMilliseconds}}
                  },
                  "endpoints": []
                }
              ]
            }
            """;

        var exception = Assert.Throws<InvalidDataException>(() => RuntimeConfigurationLoader.Parse(json, Path.GetTempPath()));
        Assert.Contains("serial.responseTimeoutMilliseconds", exception.Message);
    }

    [Fact]
    public void ParseRejectsDuplicateBusIdentifiers()
    {
        var exception = Assert.Throws<InvalidDataException>(() => RuntimeConfigurationLoader.Parse(
            """
            {
              "persistence": { "databasePath": "tr2.db" },
              "buses": [
                { "id": "bus-1", "endpoints": [] },
                { "id": "bus-1", "endpoints": [] }
              ]
            }
            """,
            Path.GetTempPath()));

        Assert.Contains("Duplicate bus id", exception.Message);
    }

    [Fact]
    public void ParseRejectsDuplicateEndpointWithinBus()
    {
        var exception = Assert.Throws<InvalidDataException>(() => RuntimeConfigurationLoader.Parse(
            """
            {
              "persistence": { "databasePath": "tr2.db" },
              "buses": [
                { "id": "bus-1", "endpoints": [7, 7] }
              ]
            }
            """,
            Path.GetTempPath()));

        Assert.Contains("Duplicate endpoint address", exception.Message);
    }

    [Theory]
    [InlineData(-1)]
    [InlineData(256)]
    public void ParseRejectsEndpointOutsideModbusAddressRepresentation(int address)
    {
        var json = $$"""
            {
              "persistence": { "databasePath": "tr2.db" },
              "buses": [
                { "id": "bus-1", "endpoints": [{{address}}] }
              ]
            }
            """;

        var exception = Assert.Throws<InvalidDataException>(() => RuntimeConfigurationLoader.Parse(
            json,
            Path.GetTempPath()));

        Assert.Contains("cannot be represented by ModbusAddress", exception.Message);
    }

    [Fact]
    public void ParseRejectsUnknownJsonMembers()
    {
        Assert.Throws<InvalidDataException>(() => RuntimeConfigurationLoader.Parse(
            """
            {
              "persistence": { "databasePath": "tr2.db" },
              "buses": [],
              "unexpected": true
            }
            """,
            Path.GetTempPath()));
    }
}
