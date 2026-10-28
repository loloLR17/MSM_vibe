namespace TR2.Supervision.Service;

// Pure, independently testable mapping. No target I/O or persistence here.
public static class LiveOperatorConfiguration
{
    public static uint Crc(ushort[] registers)
    {
        uint crc = 0xFFFFFFFF;
        foreach (var word in registers)
            foreach (var value in new byte[] { (byte)(word >> 8), (byte)word })
            {
                crc ^= value;
                for (var bit = 0; bit < 8; bit++) crc = (crc & 1) != 0 ? (crc >> 1) ^ 0xEDB88320 : crc >> 1;
            }
        return crc ^ 0xFFFFFFFF;
    }
    public static ushort[] Prepare8G(ushort[] active)
    {
        if (active.Length != 76 || active[0] != 26667 || active[2] > 3 || active[3] != 2)
            throw new InvalidDataException("Unsupported active configuration; no staging write.");
        var p = new ushort[84];
        p[0] = active[0]; p[2] = active[1]; p[3] = 2; p[4] = active[3]; p[5] = active[4]; p[6] = active[5];
        Array.Copy(active, 6, p, 7, 5); Array.Copy(active, 16, p, 24, 7);
        Array.Copy(active, 28, p, 40, 40);
        return p;
    }
}
