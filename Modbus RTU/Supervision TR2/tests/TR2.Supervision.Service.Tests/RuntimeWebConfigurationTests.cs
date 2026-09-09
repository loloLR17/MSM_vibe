using TR2.Supervision.Service;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class RuntimeWebConfigurationTests
{
    [Fact]
    public void OmittedWebSectionKeepsWebDisabledOnLoopbackDefaults()
    {
        var configuration = RuntimeConfigurationLoader.Parse(
            """
            {
              "persistence": { "databasePath": "tr2.db" },
              "buses": []
            }
            """,
            Path.GetTempPath());

        Assert.False(configuration.Web.Enabled);
        Assert.Equal(new Uri("http://127.0.0.1:5080"), configuration.Web.ListenUri);
        Assert.False(configuration.Web.AllowRemote);
        Assert.Equal(TimeSpan.FromSeconds(5), configuration.Web.FreshnessAgingAfter);
        Assert.Equal(TimeSpan.FromSeconds(30), configuration.Web.FreshnessStaleAfter);
    }

    [Fact]
    public void EnabledLoopbackWebConfigurationIsParsedExplicitly()
    {
        var configuration = RuntimeConfigurationLoader.Parse(
            """
            {
              "persistence": { "databasePath": "tr2.db" },
              "buses": [],
              "web": {
                "enabled": true,
                "listenUri": "http://127.0.0.1:5091",
                "allowRemote": false,
                "freshnessAgingAfterMilliseconds": 4000,
                "freshnessStaleAfterMilliseconds": 20000
              }
            }
            """,
            Path.GetTempPath());

        Assert.True(configuration.Web.Enabled);
        Assert.Equal(new Uri("http://127.0.0.1:5091"), configuration.Web.ListenUri);
        Assert.False(configuration.Web.AllowRemote);
        Assert.Equal(TimeSpan.FromSeconds(4), configuration.Web.FreshnessAgingAfter);
        Assert.Equal(TimeSpan.FromSeconds(20), configuration.Web.FreshnessStaleAfter);
    }

    [Fact]
    public void RemoteWebListeningRequiresExplicitAllowRemote()
    {
        var exception = Assert.Throws<InvalidDataException>(() => RuntimeConfigurationLoader.Parse(
            """
            {
              "persistence": { "databasePath": "tr2.db" },
              "buses": [],
              "web": {
                "enabled": true,
                "listenUri": "http://0.0.0.0:5080"
              }
            }
            """,
            Path.GetTempPath()));

        Assert.Contains("allowRemote=true", exception.Message, StringComparison.Ordinal);
    }

    [Fact]
    public void WebStaleThresholdMustBeGreaterThanAgingThreshold()
    {
        var exception = Assert.Throws<InvalidDataException>(() => RuntimeConfigurationLoader.Parse(
            """
            {
              "persistence": { "databasePath": "tr2.db" },
              "buses": [],
              "web": {
                "freshnessAgingAfterMilliseconds": 10000,
                "freshnessStaleAfterMilliseconds": 10000
              }
            }
            """,
            Path.GetTempPath()));

        Assert.Contains("must be greater", exception.Message, StringComparison.Ordinal);
    }
}
