using System.Text.Json;
using System.Text.Json.Serialization;
using TR2.Domain;
using TR2.Persistence.Sqlite;
using TR2.Supervision.Web;

namespace TR2.Supervision.Service;

public sealed record RuntimeConfiguration(
    SqlitePersistenceOptions Persistence,
    IReadOnlyList<RuntimeBusConfiguration> Buses,
    RuntimePollingPolicy Polling,
    RuntimeReconnectPolicy? Reconnect,
    RuntimeWebConfiguration Web);

public sealed record RuntimeWebConfiguration(
    bool Enabled,
    Uri ListenUri,
    bool AllowRemote,
    TimeSpan FreshnessAgingAfter,
    TimeSpan FreshnessStaleAfter)
{
    public static RuntimeWebConfiguration Default { get; } = new(
        Enabled: false,
        new Uri("http://127.0.0.1:5080"),
        AllowRemote: false,
        TimeSpan.FromSeconds(5),
        TimeSpan.FromSeconds(30));
}

public sealed record RuntimeBusConfiguration(
    SerialBus Bus,
    IReadOnlyList<TR2Endpoint> Endpoints)
{
    public RuntimeSerialPortConfiguration? Serial { get; init; }
}

public enum RuntimeSerialParity
{
    None,
    Odd,
    Even,
    Mark,
    Space
}

public enum RuntimeSerialStopBits
{
    One,
    Two,
    OnePointFive
}

public sealed record RuntimeSerialPortConfiguration(
    string PortName,
    int BaudRate,
    int DataBits,
    RuntimeSerialParity Parity,
    RuntimeSerialStopBits StopBits,
    TimeSpan ResponseTimeout);

public static class RuntimeConfigurationLoader
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNameCaseInsensitive = true,
        UnmappedMemberHandling = JsonUnmappedMemberHandling.Disallow
    };

    public static RuntimeConfiguration Load(string configurationPath)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(configurationPath);

        var fullPath = Path.GetFullPath(configurationPath);
        var json = File.ReadAllText(fullPath);
        var baseDirectory = Path.GetDirectoryName(fullPath) ?? Directory.GetCurrentDirectory();
        return Parse(json, baseDirectory);
    }

    public static RuntimeConfiguration Parse(string json, string baseDirectory)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(json);
        ArgumentException.ThrowIfNullOrWhiteSpace(baseDirectory);

        RuntimeConfigurationDocument document;
        try
        {
            document = JsonSerializer.Deserialize<RuntimeConfigurationDocument>(json, JsonOptions)
                ?? throw new InvalidDataException("Runtime configuration document is empty.");
        }
        catch (JsonException exception)
        {
            throw new InvalidDataException("Runtime configuration JSON is invalid.", exception);
        }

        if (document.Persistence is null || string.IsNullOrWhiteSpace(document.Persistence.DatabasePath))
        {
            throw new InvalidDataException("persistence.databasePath must be provided.");
        }

        var databasePath = document.Persistence.DatabasePath;
        if (!Path.IsPathRooted(databasePath))
        {
            databasePath = Path.Combine(Path.GetFullPath(baseDirectory), databasePath);
        }

        var persistence = new SqlitePersistenceOptions(databasePath);
        var configuredBuses = document.Buses ?? [];
        var busIds = new HashSet<string>(StringComparer.Ordinal);
        var buses = new List<RuntimeBusConfiguration>(configuredBuses.Count);

        foreach (var configuredBus in configuredBuses)
        {
            if (configuredBus is null || string.IsNullOrWhiteSpace(configuredBus.Id))
            {
                throw new InvalidDataException("Each bus must define a non-empty id.");
            }

            if (!busIds.Add(configuredBus.Id))
            {
                throw new InvalidDataException($"Duplicate bus id '{configuredBus.Id}'.");
            }

            RuntimeSerialPortConfiguration? serial = null;
            if (configuredBus.Serial is not null)
            {
                serial = CreateSerialConfiguration(configuredBus.Id, configuredBus.Serial);
            }

            var bus = new SerialBus(configuredBus.Id);
            var addresses = new HashSet<byte>();
            var configuredEndpoints = configuredBus.Endpoints ?? [];
            var endpoints = new List<TR2Endpoint>(configuredEndpoints.Count);

            foreach (var addressValue in configuredEndpoints)
            {
                if (addressValue is < byte.MinValue or > byte.MaxValue)
                {
                    throw new InvalidDataException(
                        $"Endpoint address {addressValue} on bus '{configuredBus.Id}' cannot be represented by ModbusAddress.");
                }

                var address = (byte)addressValue;
                if (!addresses.Add(address))
                {
                    throw new InvalidDataException(
                        $"Duplicate endpoint address {address} on bus '{configuredBus.Id}'.");
                }

                endpoints.Add(new TR2Endpoint(bus, new ModbusAddress(address)));
            }

            buses.Add(new RuntimeBusConfiguration(bus, endpoints)
            {
                Serial = serial
            });
        }

        var polling = CreatePollingPolicy(document.Polling);
        var reconnect = CreateReconnectPolicy(document.Reconnect);
        var web = CreateWebConfiguration(document.Web);
        return new RuntimeConfiguration(persistence, buses, polling, reconnect, web);
    }

    private static RuntimeSerialPortConfiguration CreateSerialConfiguration(string busId, SerialDocument document)
    {
        if (string.IsNullOrWhiteSpace(document.PortName))
        {
            throw new InvalidDataException($"serial.portName must be provided for bus '{busId}'.");
        }

        var baudRate = RequiredPositive(document.BaudRate, $"serial.baudRate for bus '{busId}'");
        var dataBits = RequiredPositive(document.DataBits, $"serial.dataBits for bus '{busId}'");
        var timeoutMilliseconds = RequiredPositive(
            document.ResponseTimeoutMilliseconds,
            $"serial.responseTimeoutMilliseconds for bus '{busId}'");

        var parity = ParseEnum<RuntimeSerialParity>(document.Parity, $"serial.parity for bus '{busId}'");
        var stopBits = ParseEnum<RuntimeSerialStopBits>(document.StopBits, $"serial.stopBits for bus '{busId}'");

        return new RuntimeSerialPortConfiguration(
            document.PortName,
            baudRate,
            dataBits,
            parity,
            stopBits,
            TimeSpan.FromMilliseconds(timeoutMilliseconds));
    }

    private static RuntimeReconnectPolicy? CreateReconnectPolicy(ReconnectDocument? document)
    {
        if (document is null)
        {
            return null;
        }

        var intervalMilliseconds = RequiredPositive(
            document.IntervalMilliseconds,
            "reconnect.intervalMilliseconds");

        return new RuntimeReconnectPolicy(TimeSpan.FromMilliseconds(intervalMilliseconds));
    }

    private static RuntimeWebConfiguration CreateWebConfiguration(WebDocument? document)
    {
        var defaults = RuntimeWebConfiguration.Default;
        if (document is null)
        {
            return defaults;
        }

        Uri listenUri;
        try
        {
            listenUri = string.IsNullOrWhiteSpace(document.ListenUri)
                ? defaults.ListenUri
                : new Uri(document.ListenUri, UriKind.Absolute);
        }
        catch (UriFormatException exception)
        {
            throw new InvalidDataException("web.listenUri must be a valid absolute HTTP URI.", exception);
        }

        var allowRemote = document.AllowRemote ?? defaults.AllowRemote;
        try
        {
            _ = new SupervisionWebOptions(listenUri, allowRemote);
        }
        catch (ArgumentException exception)
        {
            throw new InvalidDataException($"Invalid web configuration: {exception.Message}", exception);
        }

        var agingAfter = PositiveMilliseconds(
            document.FreshnessAgingAfterMilliseconds,
            defaults.FreshnessAgingAfter,
            "web.freshnessAgingAfterMilliseconds");
        var staleAfter = PositiveMilliseconds(
            document.FreshnessStaleAfterMilliseconds,
            defaults.FreshnessStaleAfter,
            "web.freshnessStaleAfterMilliseconds");

        if (staleAfter <= agingAfter)
        {
            throw new InvalidDataException(
                "web.freshnessStaleAfterMilliseconds must be greater than web.freshnessAgingAfterMilliseconds.");
        }

        return new RuntimeWebConfiguration(
            document.Enabled ?? defaults.Enabled,
            listenUri,
            allowRemote,
            agingAfter,
            staleAfter);
    }

    private static int RequiredPositive(int? value, string name)
    {
        if (value is null || value <= 0)
        {
            throw new InvalidDataException($"{name} must be provided and greater than zero.");
        }

        return value.Value;
    }

    private static TEnum ParseEnum<TEnum>(string? value, string name)
        where TEnum : struct, Enum
    {
        if (string.IsNullOrWhiteSpace(value) || !Enum.TryParse<TEnum>(value, true, out var parsed))
        {
            throw new InvalidDataException(
                $"{name} must be one of: {string.Join(", ", Enum.GetNames<TEnum>())}.");
        }

        return parsed;
    }

    private static RuntimePollingPolicy CreatePollingPolicy(PollingDocument? document)
    {
        var defaults = RuntimePollingPolicy.Default;
        return new RuntimePollingPolicy(
            PositiveMilliseconds(document?.StaticRetryMilliseconds, defaults.StaticRetryInterval, "polling.staticRetryMilliseconds"),
            PositiveMilliseconds(document?.FastMilliseconds, defaults.FastInterval, "polling.fastMilliseconds"),
            PositiveMilliseconds(document?.MediumMilliseconds, defaults.MediumInterval, "polling.mediumMilliseconds"),
            PositiveMilliseconds(document?.SlowMilliseconds, defaults.SlowInterval, "polling.slowMilliseconds"),
            PositiveMilliseconds(document?.ScanMilliseconds, defaults.ScanInterval, "polling.scanMilliseconds"));
    }

    private static TimeSpan PositiveMilliseconds(int? configured, TimeSpan fallback, string name)
    {
        if (configured is null)
        {
            return fallback;
        }

        if (configured <= 0)
        {
            throw new InvalidDataException($"{name} must be greater than zero.");
        }

        return TimeSpan.FromMilliseconds(configured.Value);
    }

    private sealed class RuntimeConfigurationDocument
    {
        public RuntimeConfigurationDocument() { }
        public PersistenceDocument? Persistence { get; init; }
        public List<BusDocument?>? Buses { get; init; }
        public PollingDocument? Polling { get; init; }
        public ReconnectDocument? Reconnect { get; init; }
        public WebDocument? Web { get; init; }
    }

    private sealed class PersistenceDocument
    {
        public PersistenceDocument() { }
        public string? DatabasePath { get; init; }
    }

    private sealed class BusDocument
    {
        public BusDocument() { }
        public string? Id { get; init; }
        public SerialDocument? Serial { get; init; }
        public List<int>? Endpoints { get; init; }
    }

    private sealed class SerialDocument
    {
        public SerialDocument() { }
        public string? PortName { get; init; }
        public int? BaudRate { get; init; }
        public int? DataBits { get; init; }
        public string? Parity { get; init; }
        public string? StopBits { get; init; }
        public int? ResponseTimeoutMilliseconds { get; init; }
    }

    private sealed class PollingDocument
    {
        public PollingDocument() { }
        public int? StaticRetryMilliseconds { get; init; }
        public int? FastMilliseconds { get; init; }
        public int? MediumMilliseconds { get; init; }
        public int? SlowMilliseconds { get; init; }
        public int? ScanMilliseconds { get; init; }
    }

    private sealed class ReconnectDocument
    {
        public ReconnectDocument() { }
        public int? IntervalMilliseconds { get; init; }
    }

    private sealed class WebDocument
    {
        public WebDocument() { }
        public bool? Enabled { get; init; }
        public string? ListenUri { get; init; }
        public bool? AllowRemote { get; init; }
        public int? FreshnessAgingAfterMilliseconds { get; init; }
        public int? FreshnessStaleAfterMilliseconds { get; init; }
    }
}
