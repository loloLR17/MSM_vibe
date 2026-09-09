using TR2.Domain;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class RuntimeDiagnosticSnapshotTests
{
    [Fact]
    public void CaptureReportsExistingRuntimeAuthoritiesWithoutInventingState()
    {
        var databasePath = Path.Combine(
            Path.GetTempPath(),
            $"tr2-supervision-s5d-{Guid.NewGuid():N}.db");
        var escapedDatabasePath = databasePath.Replace("\\", "\\\\");
        var configuration = RuntimeConfigurationLoader.Parse(
            $$"""
            {
              "persistence": { "databasePath": "{{escapedDatabasePath}}" },
              "buses": [
                { "id": "bus-b", "endpoints": [7] },
                { "id": "bus-a", "endpoints": [2, 1] }
              ]
            }
            """,
            Path.GetTempPath());

        var runtime = PhysicalSupervisionRuntimeFactory.Create(
            configuration,
            supportedProtocolVersion: 1);

        var snapshot = RuntimeDiagnosticSnapshot.Capture(runtime);

        Assert.Equal(SupervisionRuntimeState.Created, snapshot.HostState);
        Assert.False(snapshot.IsReady);
        Assert.Empty(snapshot.ConnectedBusIds);
        Assert.Collection(
            snapshot.Endpoints,
            endpoint => AssertEndpoint(endpoint, "bus-a", 1),
            endpoint => AssertEndpoint(endpoint, "bus-a", 2),
            endpoint => AssertEndpoint(endpoint, "bus-b", 7));
    }

    private static void AssertEndpoint(
        RuntimeEndpointDiagnostic endpoint,
        string expectedBusId,
        byte expectedAddress)
    {
        Assert.Equal(expectedBusId, endpoint.BusId);
        Assert.Equal(expectedAddress, endpoint.Address);
        Assert.Equal(TR2SessionState.Unidentified, endpoint.SessionState);
        Assert.Null(endpoint.DeviceId);
    }
}
