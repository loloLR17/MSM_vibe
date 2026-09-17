#include <stdint.h>

#include "stm32u5xx_hal.h"
#include "stm32_runtime_platform.h"

typedef struct {
    ResetCause boot_reset_cause;
    bool initialized;
} Stm32RuntimePlatformContext;

static Stm32RuntimePlatformContext g_runtime_platform;

static MonotonicTimeMs stm32_now_ms(void *context)
{
    Stm32RuntimePlatformContext *platform = (Stm32RuntimePlatformContext *)context;

    if (platform == NULL || !platform->initialized) {
        return 0u;
    }

    return (MonotonicTimeMs)HAL_GetTick();
}

static ResetCause stm32_reset_cause_get(void *context)
{
    Stm32RuntimePlatformContext *platform = (Stm32RuntimePlatformContext *)context;

    if (platform == NULL || !platform->initialized) {
        return RESET_CAUSE_UNKNOWN;
    }

    return platform->boot_reset_cause;
}

static ResetCause capture_reset_cause(void)
{
    ResetCause cause = RESET_CAUSE_UNKNOWN;

#if defined(RCC_CSR_IWDGRSTF)
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != 0u) {
        cause = RESET_CAUSE_WATCHDOG;
    }
#endif
#if defined(RCC_CSR_WWDGRSTF)
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != 0u) {
        cause = RESET_CAUSE_WATCHDOG;
    }
#endif
#if defined(RCC_CSR_SFTRSTF)
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != 0u) {
        cause = RESET_CAUSE_SOFTWARE;
    }
#endif
#if defined(RCC_CSR_PINRSTF)
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != 0u) {
        cause = RESET_CAUSE_EXTERNAL;
    }
#endif
#if defined(RCC_CSR_BORRSTF)
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST) != 0u) {
        cause = RESET_CAUSE_BROWNOUT;
    }
#endif
#if defined(RCC_CSR_OBLRSTF)
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_OBLRST) != 0u && cause == RESET_CAUSE_UNKNOWN) {
        cause = RESET_CAUSE_SOFTWARE;
    }
#endif
#if defined(RCC_CSR_PWRRSTF)
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PWRRST) != 0u && cause == RESET_CAUSE_UNKNOWN) {
        cause = RESET_CAUSE_POWER_ON;
    }
#endif

    return cause;
}

Tr2Result stm32_runtime_platform_init(void)
{
    g_runtime_platform.boot_reset_cause = capture_reset_cause();
    g_runtime_platform.initialized = true;
    __HAL_RCC_CLEAR_RESET_FLAGS();
    return TR2_OK;
}

MonotonicClock stm32_runtime_monotonic_clock(void)
{
    MonotonicClock clock = { &g_runtime_platform, stm32_now_ms };
    return clock;
}

ResetCauseProvider stm32_runtime_reset_cause_provider(void)
{
    ResetCauseProvider provider = { &g_runtime_platform, stm32_reset_cause_get };
    return provider;
}
