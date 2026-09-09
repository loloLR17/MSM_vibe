using System.Text.Json.Serialization;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using TR2.Application;

namespace TR2.Supervision.Web;

public sealed record SupervisionWebOptions
{
    public SupervisionWebOptions(Uri listenUri, bool allowRemote)
    {
        ArgumentNullException.ThrowIfNull(listenUri);

        if (!listenUri.IsAbsoluteUri ||
            !string.Equals(listenUri.Scheme, Uri.UriSchemeHttp, StringComparison.OrdinalIgnoreCase))
        {
            throw new ArgumentException("listenUri must be an absolute HTTP URI.", nameof(listenUri));
        }

        if (!string.IsNullOrEmpty(listenUri.UserInfo) ||
            (!string.IsNullOrEmpty(listenUri.AbsolutePath) && listenUri.AbsolutePath != "/") ||
            !string.IsNullOrEmpty(listenUri.Query) ||
            !string.IsNullOrEmpty(listenUri.Fragment))
        {
            throw new ArgumentException(
                "listenUri must contain only scheme, host and optional port.",
                nameof(listenUri));
        }

        if (!allowRemote && !listenUri.IsLoopback)
        {
            throw new ArgumentException(
                "Non-loopback HTTP listening requires explicit allowRemote=true.",
                nameof(listenUri));
        }

        ListenUri = listenUri;
        AllowRemote = allowRemote;
    }

    public Uri ListenUri { get; }

    public bool AllowRemote { get; }
}

public sealed record FleetReadResponse(
    DateTimeOffset ObservedAt,
    IReadOnlyList<IhmDeviceReadModel> Devices);

public sealed class SupervisionWebHost
{
    private readonly SupervisionWebOptions _options;
    private readonly SupervisionReadProjection _projection;
    private readonly TimeProvider _timeProvider;

    public SupervisionWebHost(
        SupervisionWebOptions options,
        SupervisionReadProjection projection,
        TimeProvider? timeProvider = null)
    {
        ArgumentNullException.ThrowIfNull(options);
        ArgumentNullException.ThrowIfNull(projection);

        _options = options;
        _projection = projection;
        _timeProvider = timeProvider ?? TimeProvider.System;
    }

    public WebApplication CreateApplication()
    {
        var builder = WebApplication.CreateSlimBuilder(Array.Empty<string>());
        builder.WebHost.UseUrls(_options.ListenUri.ToString());
        builder.Services.ConfigureHttpJsonOptions(options =>
            options.SerializerOptions.Converters.Add(new JsonStringEnumConverter()));

        var application = builder.Build();
        application.MapGet("/api/v1/fleet", () =>
        {
            var observedAt = _timeProvider.GetUtcNow();
            return Results.Ok(new FleetReadResponse(
                observedAt,
                _projection.GetFleet(observedAt)));
        });

        return application;
    }

    public async Task RunAsync(CancellationToken cancellationToken)
    {
        await using var application = CreateApplication();
        await application.StartAsync(cancellationToken);
        await application.WaitForShutdownAsync(cancellationToken);
    }
}
