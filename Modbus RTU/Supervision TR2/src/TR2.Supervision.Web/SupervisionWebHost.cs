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

public sealed record DeviceReadResponse(
    DateTimeOffset ObservedAt,
    IhmDeviceReadModel Device);

public sealed class SupervisionWebHost
{
    private const int MaxRequestIdentityLength = 128;
    private readonly SupervisionWebOptions _options;
    private readonly SupervisionReadProjection _projection;
    private readonly TimeProvider _timeProvider;
    private readonly ISupervisionSystemReadSource? _systemReadSource;
    private readonly ISupervisionCommandSink? _commandSink;

    public SupervisionWebHost(
        SupervisionWebOptions options,
        SupervisionReadProjection projection,
        TimeProvider? timeProvider = null,
        ISupervisionSystemReadSource? systemReadSource = null,
        ISupervisionCommandSink? commandSink = null)
    {
        ArgumentNullException.ThrowIfNull(options);
        ArgumentNullException.ThrowIfNull(projection);

        _options = options;
        _projection = projection;
        _timeProvider = timeProvider ?? TimeProvider.System;
        _systemReadSource = systemReadSource;
        _commandSink = commandSink;
    }

    public WebApplication CreateApplication()
    {
        var builder = WebApplication.CreateSlimBuilder(Array.Empty<string>());
        builder.WebHost.UseUrls(_options.ListenUri.ToString());
        builder.Services.ConfigureHttpJsonOptions(options =>
            options.SerializerOptions.Converters.Add(new JsonStringEnumConverter(allowIntegerValues: false)));

        var application = builder.Build();
        application.UseDefaultFiles();
        application.UseStaticFiles();
        application.MapGet("/api/v1/fleet", () =>
        {
            var observedAt = _timeProvider.GetUtcNow();
            return Results.Ok(new FleetReadResponse(
                observedAt,
                _projection.GetFleet(observedAt)));
        });
        application.MapGet("/api/v1/devices/{deviceId:long}", (long deviceId) =>
        {
            if (deviceId < 0 || deviceId > uint.MaxValue)
            {
                return Results.NotFound();
            }

            var observedAt = _timeProvider.GetUtcNow();
            var device = _projection.GetDevice((uint)deviceId, observedAt);
            return device is null
                ? Results.NotFound()
                : Results.Ok(new DeviceReadResponse(observedAt, device));
        });
        application.MapGet("/api/v1/system", () =>
        {
            if (_systemReadSource is null)
            {
                return Results.NotFound();
            }

            return Results.Ok(new SystemReadResponse(
                _timeProvider.GetUtcNow(),
                _systemReadSource.Read()));
        });
        application.MapPost("/api/v1/devices/{deviceId:long}/commands", async (
            long deviceId,
            QueueB5CommandRequest request,
            CancellationToken cancellationToken) =>
        {
            if (_commandSink is null)
            {
                return Results.NotFound();
            }

            if (deviceId < 0 || deviceId > uint.MaxValue)
            {
                return Results.NotFound();
            }

            if (!TryCreateSubmission(request, out var submission, out var validationMessage))
            {
                return Results.BadRequest(new QueueB5CommandRejectedResponse(
                    "InvalidRequest",
                    validationMessage));
            }

            var acceptedAt = _timeProvider.GetUtcNow();
            var result = await _commandSink.QueueAsync(
                (uint)deviceId,
                request.RequestIdentity!,
                submission!,
                acceptedAt,
                cancellationToken);

            return result.Status switch
            {
                IhmCommandQueueStatus.Accepted => Results.Accepted(
                    $"/api/v1/devices/{deviceId}",
                    new QueueB5CommandAcceptedResponse(
                        acceptedAt,
                        result.WorkId!.Value,
                        result.DeviceId!.Value,
                        result.TransactionId!.Value,
                        submission!.Command,
                        request.RequestIdentity!)),
                IhmCommandQueueStatus.DeviceNotFound => Results.NotFound(),
                IhmCommandQueueStatus.DuplicateRequestIdentity => Results.Conflict(
                    new QueueB5CommandRejectedResponse(
                        "DuplicateRequestIdentity",
                        result.Detail ?? "requestIdentity was already used for this device.")),
                IhmCommandQueueStatus.NotReady => Results.Json(
                    new QueueB5CommandRejectedResponse(
                        "RuntimeNotReady",
                        result.Detail ?? "The supervision runtime is not ready."),
                    statusCode: StatusCodes.Status503ServiceUnavailable),
                _ => Results.Conflict(new QueueB5CommandRejectedResponse(
                    "CommandConflict",
                    result.Detail ?? "The command cannot be queued in the current runtime state."))
            };
        });

        return application;
    }

    public async Task RunAsync(CancellationToken cancellationToken)
    {
        await using var application = CreateApplication();
        await application.StartAsync(cancellationToken);
        await application.WaitForShutdownAsync(cancellationToken);
    }

    private static bool TryCreateSubmission(
        QueueB5CommandRequest request,
        out IhmB5CommandSubmission? submission,
        out string validationMessage)
    {
        submission = null;
        validationMessage = string.Empty;

        if (string.IsNullOrWhiteSpace(request.RequestIdentity) ||
            request.RequestIdentity.Length > MaxRequestIdentityLength ||
            request.Command is null ||
            !Enum.IsDefined(request.Command.Value))
        {
            validationMessage = "requestIdentity must contain 1..128 non-whitespace characters and command must be a supported S7-G command.";
            return false;
        }

        if (request.Command == IhmB5Command.AcknowledgeFault)
        {
            if (request.ConfirmProtectedCommand is not null)
            {
                validationMessage = "AcknowledgeFault does not accept confirmProtectedCommand.";
                return false;
            }

            if (request.AcknowledgeAll == false && request.FaultCode.HasValue)
            {
                submission = new IhmB5CommandSubmission(
                    request.Command.Value,
                    request.FaultCode.Value,
                    AcknowledgeAll: false);
                return true;
            }

            if (request.AcknowledgeAll == true && !request.FaultCode.HasValue)
            {
                submission = new IhmB5CommandSubmission(
                    request.Command.Value,
                    AcknowledgeAll: true);
                return true;
            }

            validationMessage = "AcknowledgeFault requires either acknowledgeAll=false with faultCode, or acknowledgeAll=true without faultCode.";
            return false;
        }

        if (request.Command == IhmB5Command.SoftwareReset)
        {
            if (request.FaultCode.HasValue || request.AcknowledgeAll.HasValue || request.ConfirmProtectedCommand != true)
            {
                validationMessage = "SoftwareReset requires confirmProtectedCommand=true and accepts no fault acknowledgement fields.";
                return false;
            }

            submission = new IhmB5CommandSubmission(
                request.Command.Value,
                ConfirmProtectedCommand: true);
            return true;
        }

        if (request.FaultCode.HasValue || request.AcknowledgeAll.HasValue || request.ConfirmProtectedCommand.HasValue)
        {
            validationMessage = "This command does not accept fault acknowledgement or protected-command confirmation fields.";
            return false;
        }

        submission = new IhmB5CommandSubmission(request.Command.Value);
        return true;
    }
}
