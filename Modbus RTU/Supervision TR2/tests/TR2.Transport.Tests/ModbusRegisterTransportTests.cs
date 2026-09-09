using System.IO;
using TR2.Transport;
using Xunit;

namespace TR2.Transport.Tests;

public sealed class ModbusRegisterTransportTests
{
    [Fact]
    public async Task ReadRegistersDelegatesFc03Parameters()
    {
        var client = new FakeModbusRegisterClient
        {
            ReadResult = [0x1234, 0x5678]
        };
        var transport = new ModbusRegisterTransport("bus-1", client);

        var result = await transport.ReadRegistersAsync("bus-1", 7, 100, 2);

        Assert.Equal(new ushort[] { 0x1234, 0x5678 }, result);
        Assert.Equal((byte)7, client.LastReadUnitAddress);
        Assert.Equal((ushort)100, client.LastReadStartAddress);
        Assert.Equal((ushort)2, client.LastReadRegisterCount);
        Assert.Equal(1, client.ReadCallCount);
    }

    [Fact]
    public async Task WriteRegistersDelegatesFc16Parameters()
    {
        var client = new FakeModbusRegisterClient();
        var transport = new ModbusRegisterTransport("bus-1", client);

        await transport.WriteRegistersAsync("bus-1", 9, 200, new ushort[] { 1, 2, 3 });

        Assert.Equal((byte)9, client.LastWriteUnitAddress);
        Assert.Equal((ushort)200, client.LastWriteStartAddress);
        Assert.Equal(new ushort[] { 1, 2, 3 }, client.LastWriteValues);
        Assert.Equal(1, client.WriteCallCount);
    }

    [Fact]
    public async Task TimeoutIsWrappedAsNeutralTransportFailure()
    {
        var client = new FakeModbusRegisterClient
        {
            ReadException = new TimeoutException("no response")
        };
        var transport = new ModbusRegisterTransport("bus-1", client);

        var exception = await Assert.ThrowsAsync<ModbusTransportFailureException>(async () =>
            await transport.ReadRegistersAsync("bus-1", 1, 0, 1));

        Assert.Equal(ModbusTransportFailureKind.Timeout, exception.Kind);
        Assert.IsType<TimeoutException>(exception.InnerException);
    }

    [Fact]
    public async Task IoFailureIsWrappedAsNeutralTransportFailure()
    {
        var client = new FakeModbusRegisterClient
        {
            ReadException = new IOException("port removed")
        };
        var transport = new ModbusRegisterTransport("bus-1", client);

        var exception = await Assert.ThrowsAsync<ModbusTransportFailureException>(async () =>
            await transport.ReadRegistersAsync("bus-1", 1, 0, 1));

        Assert.Equal(ModbusTransportFailureKind.Io, exception.Kind);
        Assert.IsType<IOException>(exception.InnerException);
    }

    [Fact]
    public async Task ReadRejectsWorkForAnotherBusBeforeCallingClient()
    {
        var client = new FakeModbusRegisterClient();
        var transport = new ModbusRegisterTransport("bus-1", client);

        await Assert.ThrowsAsync<InvalidOperationException>(async () =>
            await transport.ReadRegistersAsync("bus-2", 1, 0, 1));

        Assert.Equal(0, client.ReadCallCount);
    }

    [Fact]
    public async Task WriteRejectsEmptyValuesBeforeCallingClient()
    {
        var client = new FakeModbusRegisterClient();
        var transport = new ModbusRegisterTransport("bus-1", client);

        await Assert.ThrowsAsync<ArgumentException>(async () =>
            await transport.WriteRegistersAsync("bus-1", 1, 0, Array.Empty<ushort>()));

        Assert.Equal(0, client.WriteCallCount);
    }

    [Fact]
    public async Task PreCancelledReadDoesNotStartModbusOperation()
    {
        var client = new FakeModbusRegisterClient();
        var transport = new ModbusRegisterTransport("bus-1", client);
        using var cancellation = new CancellationTokenSource();
        cancellation.Cancel();

        await Assert.ThrowsAnyAsync<OperationCanceledException>(async () =>
            await transport.ReadRegistersAsync("bus-1", 1, 0, 1, cancellation.Token));

        Assert.Equal(0, client.ReadCallCount);
    }

    [Fact]
    public async Task PreCancelledWriteDoesNotStartModbusOperation()
    {
        var client = new FakeModbusRegisterClient();
        var transport = new ModbusRegisterTransport("bus-1", client);
        using var cancellation = new CancellationTokenSource();
        cancellation.Cancel();

        await Assert.ThrowsAnyAsync<OperationCanceledException>(async () =>
            await transport.WriteRegistersAsync("bus-1", 1, 0, new ushort[] { 1 }, cancellation.Token));

        Assert.Equal(0, client.WriteCallCount);
    }

    private sealed class FakeModbusRegisterClient : IModbusRegisterClient
    {
        public ushort[] ReadResult { get; init; } = [0];
        public Exception? ReadException { get; init; }
        public Exception? WriteException { get; init; }
        public int ReadCallCount { get; private set; }
        public int WriteCallCount { get; private set; }
        public byte LastReadUnitAddress { get; private set; }
        public ushort LastReadStartAddress { get; private set; }
        public ushort LastReadRegisterCount { get; private set; }
        public byte LastWriteUnitAddress { get; private set; }
        public ushort LastWriteStartAddress { get; private set; }
        public ushort[] LastWriteValues { get; private set; } = [];

        public Task<ushort[]> ReadHoldingRegistersAsync(
            byte unitAddress,
            ushort startAddress,
            ushort registerCount)
        {
            ReadCallCount++;
            LastReadUnitAddress = unitAddress;
            LastReadStartAddress = startAddress;
            LastReadRegisterCount = registerCount;

            if (ReadException is not null)
            {
                return Task.FromException<ushort[]>(ReadException);
            }

            return Task.FromResult(ReadResult);
        }

        public Task WriteMultipleRegistersAsync(
            byte unitAddress,
            ushort startAddress,
            ushort[] values)
        {
            WriteCallCount++;
            LastWriteUnitAddress = unitAddress;
            LastWriteStartAddress = startAddress;
            LastWriteValues = values;

            return WriteException is null
                ? Task.CompletedTask
                : Task.FromException(WriteException);
        }
    }
}
