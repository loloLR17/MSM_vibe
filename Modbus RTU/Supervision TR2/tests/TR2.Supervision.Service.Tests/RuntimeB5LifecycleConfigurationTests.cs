using TR2.Supervision.Service;
using Xunit;

namespace TR2.Supervision.Service.Tests;

public sealed class RuntimeB5LifecycleConfigurationTests
{
    [Fact]
    public void ParseLeavesB5LifecycleDisabledWhenBlockIsAbsent()
    {
        var configuration = RuntimeConfigurationLoader.Parse(
            """
            {
              "persistence": { "databasePath": "tr2.db" },
              "buses": []
            }
            """,
            Path.GetTempPath());

        Assert.Null(configuration.B5);
    }

    [Fact]
    public void ParseLoadsExplicitB5LifecyclePolicy()
    {
        var configuration = RuntimeConfigurationLoader.Parse(
            """
            {
              "persistence": { "databasePath": "tr2.db" },
              "b5": {
                "postSubmitPollIntervalMilliseconds": 500,
                "postSubmitTimeoutMilliseconds": 30000,
                "reconciliationIntervalMilliseconds": 2000
              },
              "buses": []
            }
            """,
            Path.GetTempPath());

        var b5 = Assert.IsType<RuntimeB5LifecyclePolicy>(configuration.B5);
        Assert.Equal(TimeSpan.FromMilliseconds(500), b5.PostSubmitPollInterval);
        Assert.Equal(TimeSpan.FromMilliseconds(30000), b5.PostSubmitTimeout);
        Assert.Equal(TimeSpan.FromMilliseconds(2000), b5.ReconciliationInterval);
    }

    [Theory]
    [InlineData(0, 30000, 2000, "b5.postSubmitPollIntervalMilliseconds")]
    [InlineData(500, 0, 2000, "b5.postSubmitTimeoutMilliseconds")]
    [InlineData(500, 30000, 0, "b5.reconciliationIntervalMilliseconds")]
    public void ParseRejectsNonPositiveB5LifecycleValues(
        int pollMilliseconds,
        int timeoutMilliseconds,
        int reconciliationMilliseconds,
        string expectedMessage)
    {
        var json = $$"""
            {
              "persistence": { "databasePath": "tr2.db" },
              "b5": {
                "postSubmitPollIntervalMilliseconds": {{pollMilliseconds}},
                "postSubmitTimeoutMilliseconds": {{timeoutMilliseconds}},
                "reconciliationIntervalMilliseconds": {{reconciliationMilliseconds}}
              },
              "buses": []
            }
            """;

        var exception = Assert.Throws<InvalidDataException>(() =>
            RuntimeConfigurationLoader.Parse(json, Path.GetTempPath()));

        Assert.Contains(expectedMessage, exception.Message);
    }

    [Fact]
    public void ParseRejectsB5TimeoutShorterThanPollInterval()
    {
        var exception = Assert.Throws<InvalidDataException>(() =>
            RuntimeConfigurationLoader.Parse(
                """
                {
                  "persistence": { "databasePath": "tr2.db" },
                  "b5": {
                    "postSubmitPollIntervalMilliseconds": 1000,
                    "postSubmitTimeoutMilliseconds": 999,
                    "reconciliationIntervalMilliseconds": 2000
                  },
                  "buses": []
                }
                """,
                Path.GetTempPath()));

        Assert.Contains("must be greater than or equal", exception.Message);
    }

    [Fact]
    public void ParseRejectsIncompleteB5LifecycleBlock()
    {
        var exception = Assert.Throws<InvalidDataException>(() =>
            RuntimeConfigurationLoader.Parse(
                """
                {
                  "persistence": { "databasePath": "tr2.db" },
                  "b5": {
                    "postSubmitPollIntervalMilliseconds": 500,
                    "postSubmitTimeoutMilliseconds": 30000
                  },
                  "buses": []
                }
                """,
                Path.GetTempPath()));

        Assert.Contains("b5.reconciliationIntervalMilliseconds", exception.Message);
    }
}
