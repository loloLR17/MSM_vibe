using Xunit;

namespace TR2.Application.Tests;

public sealed class AssemblySmokeTests
{
    [Fact]
    public void Assembly_name_is_stable()
    {
        Assert.Equal("TR2.Application", typeof(TR2.Application.AssemblyMarker).Assembly.GetName().Name);
    }
}
