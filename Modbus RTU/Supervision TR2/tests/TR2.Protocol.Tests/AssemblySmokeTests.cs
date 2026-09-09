using Xunit;

namespace TR2.Protocol.Tests;

public sealed class AssemblySmokeTests
{
    [Fact]
    public void Assembly_name_is_stable()
    {
        Assert.Equal("TR2.Protocol", typeof(TR2.Protocol.AssemblyMarker).Assembly.GetName().Name);
    }
}
