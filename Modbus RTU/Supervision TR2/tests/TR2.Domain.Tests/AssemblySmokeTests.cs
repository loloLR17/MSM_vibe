using Xunit;

namespace TR2.Domain.Tests;

public sealed class AssemblySmokeTests
{
    [Fact]
    public void Assembly_name_is_stable()
    {
        Assert.Equal("TR2.Domain", typeof(TR2.Domain.AssemblyMarker).Assembly.GetName().Name);
    }
}
