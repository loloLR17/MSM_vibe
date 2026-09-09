using System.Text.Json;
using System.Text.Json.Serialization;
using TR2.Domain;
using TR2.Persistence.Sqlite;

namespace TR2.Supervision.Service;

public sealed record RuntimeConfiguration(
    SqlitePersistenceOptions Persistence,
    IReadOnlyList<RuntimeBusConfiguration> Buses);

public sealed record RuntimeBusConfiguration(
    SerialBus Bus,
    IReadOnlyList<TR2Endpoint> Endpoints);

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

            buses.Add(new RuntimeBusConfiguration(bus, endpoints));
        }

        return new RuntimeConfiguration(persistence, buses);
    }

    private sealed class RuntimeConfigurationDocument
    {
        public RuntimeConfigurationDocument()
        {
        }

        public PersistenceDocument? Persistence { get; init; }

        public List<BusDocument?>? Buses { get; init; }
    }

    private sealed class PersistenceDocument
    {
        public PersistenceDocument()
        {
        }

        public string? DatabasePath { get; init; }
    }

    private sealed class BusDocument
    {
        public BusDocument()
        {
        }

        public string? Id { get; init; }

        public List<int>? Endpoints { get; init; }
    }
}
