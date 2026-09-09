namespace TR2.Application;

public interface IPollingFailureClassifier
{
    bool IsCommunicationFailure(Exception exception);
}
