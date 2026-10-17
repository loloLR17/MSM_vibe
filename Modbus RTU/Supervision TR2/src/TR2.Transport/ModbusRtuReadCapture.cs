using System.Buffers.Binary;

namespace TR2.Transport;

// Offline captures only. The physical connection continues to use the existing NModbus transport.
public static class ModbusRtuReadCapture
{
    public static ushort[] Decode(ReadOnlySpan<byte> request, ReadOnlySpan<byte> response)
    {
        if (request.Length != 8 || request[0] is < 1 or > 247 || request[1] != 3 || Crc(request) != 0)
            throw new InvalidDataException("Invalid FC03 capture request/CRC.");
        var count = BinaryPrimitives.ReadUInt16BigEndian(request[4..]);
        if (count is < 1 or > 125 || response.Length != 5 + count * 2 || response[0] != request[0] ||
            response[1] != 3 || response[2] != count * 2 || Crc(response) != 0)
            throw new InvalidDataException("Invalid FC03 capture response/unit/length/CRC.");
        var words = new ushort[count];
        for (var i = 0; i < count; i++) words[i] = BinaryPrimitives.ReadUInt16BigEndian(response[(3 + i * 2)..]);
        return words;
    }
    private static ushort Crc(ReadOnlySpan<byte> bytes)
    {
        ushort crc = 65535;
        foreach (var b in bytes)
        {
            crc ^= b;
            for (var bit = 0; bit < 8; bit++) crc = (ushort)((crc >> 1) ^ ((crc & 1) != 0 ? 0xA001 : 0));
        }
        return crc;
    }
}
