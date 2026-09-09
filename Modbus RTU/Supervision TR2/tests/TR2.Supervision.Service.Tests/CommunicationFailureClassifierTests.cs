using TR2.Application;
using TR2.Transport;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class CommunicationFailureClassifierTests
{
    [Theory]
    [InlineData(ModbusTransportFailureKind.Timeout, CommunicationFailureCategory.Timeout)]
    [InlineData(ModbusTransportFailureKind.Io, CommunicationFailureCategory.Io)]
    [InlineData(ModbusTransportFailureKind.ModbusExceptionResponse, CommunicationFailureCategory.ModbusExceptionResponse)]
    public void Neutral_transport_failures_map_to_stable_supervision_categories(
        ModbusTransportFailureKind kind,
        CommunicationFailureCategory expected)
    {
        Exception inner = kind switch
        {
            ModbusTransportFailureKind.Timeout => new TimeoutException("timeout"),
            ModbusTransportFailureKind.Io => new IOException("io"),
            ModbusTransportFailureKind.ModbusExceptionResponse => new InvalidOperationException("modbus exception response"),
            _ => new InvalidOperationException("unexpected")
        };
        var exception = new ModbusTransportFailureException(kind, "transport failure", inner);

        var category = CommunicationFailureClassifier.Classify(exception);

        Assert.Equal(expected, category);
    }

    [Fact]
    public void Unknown_exception_is_not_guessed_into_a_known_category()
    {
        var category = CommunicationFailureClassifier.Classify(
            new InvalidOperationException("unexpected"));

        Assert.Equal(CommunicationFailureCategory.Unclassified, category);
    }
}
