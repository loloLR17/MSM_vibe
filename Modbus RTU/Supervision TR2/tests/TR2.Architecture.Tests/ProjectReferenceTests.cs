using System.Xml.Linq;
using Xunit;

namespace TR2.Architecture.Tests;

public sealed class ProjectReferenceTests
{
    private static readonly IReadOnlyDictionary<string, string[]> ExpectedReferences =
        new Dictionary<string, string[]>(StringComparer.Ordinal)
        {
            ["TR2.Domain"] = [],
            ["TR2.Transport"] = [],
            ["TR2.Protocol"] = ["TR2.Domain", "TR2.Transport"],
            ["TR2.Application"] = ["TR2.Domain", "TR2.Protocol"],
            ["TR2.Persistence"] = ["TR2.Application", "TR2.Domain"],
            ["TR2.Campaigns"] = ["TR2.Application", "TR2.Domain"],
            ["TR2.Supervision.Web"] = ["TR2.Application"],
            ["TR2.Supervision.Service"] =
            [
                "TR2.Application",
                "TR2.Campaigns",
                "TR2.Domain",
                "TR2.Persistence",
                "TR2.Protocol",
                "TR2.Transport"
            ]
        };

    [Fact]
    public void Production_project_references_match_S1A_boundaries()
    {
        var solutionRoot = Path.GetFullPath(
            Path.Combine(AppContext.BaseDirectory, "..", "..", "..", "..", ".."));

        foreach (var (projectName, expected) in ExpectedReferences)
        {
            var projectPath = Path.Combine(
                solutionRoot, "src", projectName, $"{projectName}.csproj");

            Assert.True(File.Exists(projectPath), $"Missing project: {projectPath}");

            var document = XDocument.Load(projectPath);
            var actual = document
                .Descendants("ProjectReference")
                .Select(reference => reference.Attribute("Include")?.Value)
                .Where(value => !string.IsNullOrWhiteSpace(value))
                .Select(value => value!.Replace('\\', '/'))
                .Select(Path.GetFileNameWithoutExtension)
                .OrderBy(value => value, StringComparer.Ordinal)
                .ToArray();

            Assert.Equal(
                expected.OrderBy(value => value, StringComparer.Ordinal),
                actual);
        }
    }
}
