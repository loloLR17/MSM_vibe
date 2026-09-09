using TR2.Application;
using TR2.Transport;

namespace TR2.Supervision.Service;

public static class CommunicationFailureClassifier
{
    public static CommunicationFailureCategory Classify(Exception exception)
    {
        ArgumentNullException.ThrowIfNull(exception);

        return exception switch
        {
            ModbusTransportFailureException transportFailure => transportFailure.Kind switch
            {
                ModbusTransportFailureKind.Timeout => CommunicationFailureCategory.Timeout,
                ModbusTransportFailureKind.Io => CommunicationFailureCategory.Io,
                _ => CommunicationFailureCategory.Unclassified
            },
            _ => CommunicationFailureCategory.Unclassified
        };
    }
}
