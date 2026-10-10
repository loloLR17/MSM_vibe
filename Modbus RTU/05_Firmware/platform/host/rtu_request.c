#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tr2/modbus/rtu_server_runtime.h"

/* Offline request generator: no serial access, reuses the production codec. */
int main(int argc, char **argv)
{
    uint8_t adu[MODBUS_RTU_ADU_MAX_SIZE];
    size_t length;
    const uint8_t read_b1[] = {3u, 0x03u, 0xE8u, 0u, 1u};
    const uint8_t unsupported[] = {6u, 0u, 0u, 0u, 1u};
    const uint8_t *pdu = read_b1;
    size_t pdu_length = sizeof(read_b1);
    char *end;
    unsigned long parsed;
    uint8_t unit;
    bool corrupt = false;

    if (argc == 3 && strcmp(argv[1], "--decode") == 0) {
        ModbusRtuAduView view;
        FILE *file = fopen(argv[2], "rb");
        int extra;
        if (file == NULL) {
            perror(argv[2]);
            return 1;
        }
        length = fread(adu, 1u, sizeof(adu), file);
        extra = fgetc(file);
        bool read_failed = ferror(file) != 0;
        fclose(file);
        if (read_failed || extra != EOF ||
            modbus_rtu_adu_decode(adu, length, &view) != MODBUS_RTU_CODEC_OK) {
            fprintf(stderr, "Invalid ADU length/CRC (or concatenated frames)\n");
            return 1;
        }
        printf("unit=%u pdu=", view.unit_id);
        for (size_t i = 0u; i < view.pdu_length; i++) printf("%02x", view.pdu[i]);
        putchar('\n');
        return 0;
    }

    if (argc != 3) {
        fprintf(stderr, "Usage: %s UNIT b1|bad-crc|other-address|unsupported (or --decode FILE)\n", argv[0]);
        return 2;
    }
    errno = 0;
    parsed = strtoul(argv[1], &end, 10);
    if (errno != 0 || end == argv[1] || *end != '\0' || parsed < 1u || parsed > 247u) {
        fprintf(stderr, "UNIT must be 1..247\n");
        return 2;
    }
    unit = (uint8_t)parsed;
    if (strcmp(argv[2], "unsupported") == 0) {
        pdu = unsupported;
        pdu_length = sizeof(unsupported);
    } else if (strcmp(argv[2], "bad-crc") == 0) {
        corrupt = true;
    } else if (strcmp(argv[2], "other-address") == 0) {
        unit = unit == 247u ? 246u : (uint8_t)(unit + 1u);
    } else if (strcmp(argv[2], "b1") != 0) {
        fprintf(stderr, "Unknown case\n");
        return 2;
    }
    if (modbus_rtu_adu_encode(unit, pdu, pdu_length, adu, sizeof(adu), &length)
        != MODBUS_RTU_CODEC_OK) return 1;
    if (corrupt) adu[length - 1u] ^= 1u;
    for (size_t i = 0u; i < length; i++) printf("%02x", adu[i]);
    putchar('\n');
    return 0;
}
