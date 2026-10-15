#ifndef TR2_IIS3DWB_UNITS_H
#define TR2_IIS3DWB_UNITS_H
#include <stdint.h>
/* Caller validates FS 0..3. Preserve V1 nearest mg, ties away from zero. */
static inline int32_t tr2_iis3dwb_raw_to_mg(int16_t raw, uint16_t full_scale_code)
{
    static const int32_t sensitivity_ug_per_lsb[4] = {61, 122, 244, 488};
    int32_t micro_g = (int32_t)raw * sensitivity_ug_per_lsb[full_scale_code];

    if (micro_g >= 0) {
        return (micro_g + 500) / 1000;
    }
    return (micro_g - 500) / 1000;
}

#endif
