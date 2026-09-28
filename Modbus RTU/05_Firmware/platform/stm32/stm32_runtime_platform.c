#include <stdbool.h>
#include <stdint.h>

#include "stm32u5xx_hal.h"
#include "stm32_runtime_platform.h"

#define TR2_RTC_CONTINUITY_BACKUP_REGISTER RTC_BKP_DR0
#define TR2_RTC_CONTINUITY_MARKER UINT32_C(0x54523201)

typedef struct {
    ResetCause boot_reset_cause;
    RTC_HandleTypeDef rtc;
    bool rtc_available;
    bool initialized;
} Stm32RuntimePlatformContext;

static Stm32RuntimePlatformContext g_runtime_platform;

volatile uint32_t tr2_rtc_h3ec_init_stage = 0U;
volatile uint32_t tr2_rtc_h3ec_lse_hal_status = (uint32_t)HAL_ERROR;
volatile uint32_t tr2_rtc_h3ec_clock_hal_status = (uint32_t)HAL_ERROR;
volatile uint32_t tr2_rtc_h3ec_rtc_hal_status = (uint32_t)HAL_ERROR;
volatile uint32_t tr2_rtc_h3ec_reg_tr = 0U;
volatile uint32_t tr2_rtc_h3ec_reg_dr = 0U;
volatile uint32_t tr2_rtc_h3ec_reg_icsr = 0U;
volatile uint32_t tr2_rtc_h3ec_reg_bkp0r = 0U;


static bool is_leap_year(uint32_t year)
{
    return ((year % 4U) == 0U) &&
           (((year % 100U) != 0U) || ((year % 400U) == 0U));
}

static uint32_t days_before_year(uint32_t year)
{
    uint32_t y = year - 1U;
    return (365U * (year - 1970U)) +
           ((y / 4U) - (1969U / 4U)) -
           ((y / 100U) - (1969U / 100U)) +
           ((y / 400U) - (1969U / 400U));
}

static uint32_t days_before_month(uint32_t year, uint32_t month)
{
    static const uint16_t cumulative[12] = {
        0U, 31U, 59U, 90U, 120U, 151U, 181U, 212U, 243U, 273U, 304U, 334U
    };
    uint32_t days = cumulative[month - 1U];

    if (month > 2U && is_leap_year(year)) {
        days++;
    }
    return days;
}

static bool rtc_to_timestamp(const RTC_DateTypeDef *date,
                             const RTC_TimeTypeDef *time,
                             Tr2CivilTimestamp *timestamp)
{
    uint32_t year;
    uint64_t seconds;

    if (date == NULL || time == NULL || timestamp == NULL ||
        date->Year > 99U || date->Month < 1U || date->Month > 12U ||
        date->Date < 1U || date->Date > 31U ||
        time->Hours > 23U || time->Minutes > 59U || time->Seconds > 59U) {
        return false;
    }

    year = 2000U + date->Year;
    seconds = ((uint64_t)days_before_year(year) +
               days_before_month(year, date->Month) +
               (uint32_t)date->Date - 1U) * UINT64_C(86400);
    seconds += ((uint64_t)time->Hours * UINT64_C(3600)) +
               ((uint64_t)time->Minutes * UINT64_C(60)) +
               (uint64_t)time->Seconds;

    if (seconds > UINT32_MAX) {
        return false;
    }

    *timestamp = (Tr2CivilTimestamp)seconds;
    return true;
}

static bool timestamp_to_rtc(Tr2CivilTimestamp timestamp,
                             RTC_DateTypeDef *date,
                             RTC_TimeTypeDef *time)
{
    uint32_t days = timestamp / UINT32_C(86400);
    uint32_t seconds = timestamp % UINT32_C(86400);
    uint32_t year = 1970U;
    uint32_t month = 1U;
    static const uint8_t month_days[12] = {
        31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U
    };

    if (date == NULL || time == NULL) {
        return false;
    }

    while (year < 2000U) {
        uint32_t year_days = is_leap_year(year) ? 366U : 365U;
        if (days < year_days) {
            return false;
        }
        days -= year_days;
        year++;
    }
    while (year <= 2099U) {
        uint32_t year_days = is_leap_year(year) ? 366U : 365U;
        if (days < year_days) {
            break;
        }
        days -= year_days;
        year++;
    }
    if (year > 2099U) {
        return false;
    }

    while (month <= 12U) {
        uint32_t mdays = month_days[month - 1U];
        if (month == 2U && is_leap_year(year)) {
            mdays++;
        }
        if (days < mdays) {
            break;
        }
        days -= mdays;
        month++;
    }
    if (month > 12U) {
        return false;
    }

    *date = (RTC_DateTypeDef){0};
    date->Year = (uint8_t)(year - 2000U);
    date->Month = (uint8_t)month;
    date->Date = (uint8_t)(days + 1U);
    date->WeekDay = RTC_WEEKDAY_MONDAY;

    *time = (RTC_TimeTypeDef){0};
    time->Hours = (uint8_t)(seconds / UINT32_C(3600));
    seconds %= UINT32_C(3600);
    time->Minutes = (uint8_t)(seconds / UINT32_C(60));
    time->Seconds = (uint8_t)(seconds % UINT32_C(60));
    time->DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    time->StoreOperation = RTC_STOREOPERATION_RESET;
    return true;
}

static WallClockReadResult stm32_wall_clock_read(void *context,
                                                Tr2CivilTimestamp *timestamp)
{
    Stm32RuntimePlatformContext *platform = (Stm32RuntimePlatformContext *)context;
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;

    if (platform == NULL || timestamp == NULL || !platform->initialized ||
        !platform->rtc_available) {
        return WALL_CLOCK_UNAVAILABLE;
    }
    if (HAL_RTC_GetTime(&platform->rtc, &time, RTC_FORMAT_BIN) != HAL_OK ||
        HAL_RTC_GetDate(&platform->rtc, &date, RTC_FORMAT_BIN) != HAL_OK) {
        return WALL_CLOCK_UNAVAILABLE;
    }
    if (HAL_RTCEx_BKUPRead(&platform->rtc, TR2_RTC_CONTINUITY_BACKUP_REGISTER) !=
        TR2_RTC_CONTINUITY_MARKER) {
        return WALL_CLOCK_INVALID;
    }
    return rtc_to_timestamp(&date, &time, timestamp) ?
        WALL_CLOCK_OK : WALL_CLOCK_INVALID;
}

static Tr2Result stm32_wall_clock_set(void *context, Tr2CivilTimestamp timestamp)
{
    Stm32RuntimePlatformContext *platform = (Stm32RuntimePlatformContext *)context;
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;

    if (platform == NULL || !platform->initialized || !platform->rtc_available) {
        return TR2_ERROR_UNAVAILABLE;
    }
    if (!timestamp_to_rtc(timestamp, &date, &time)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }
    if (HAL_RTC_SetTime(&platform->rtc, &time, RTC_FORMAT_BIN) != HAL_OK ||
        HAL_RTC_SetDate(&platform->rtc, &date, RTC_FORMAT_BIN) != HAL_OK) {
        return TR2_ERROR_UNAVAILABLE;
    }

    HAL_RTCEx_BKUPWrite(&platform->rtc,
                        TR2_RTC_CONTINUITY_BACKUP_REGISTER,
                        TR2_RTC_CONTINUITY_MARKER);
    return TR2_OK;
}

static TimeContinuityEvidence stm32_time_continuity_get(void *context)
{
    Stm32RuntimePlatformContext *platform = (Stm32RuntimePlatformContext *)context;

    if (platform == NULL || !platform->initialized || !platform->rtc_available) {
        return TIME_CONTINUITY_EVIDENCE_INDETERMINATE;
    }

    return HAL_RTCEx_BKUPRead(&platform->rtc, TR2_RTC_CONTINUITY_BACKUP_REGISTER) ==
        TR2_RTC_CONTINUITY_MARKER ?
        TIME_CONTINUITY_EVIDENCE_PROVEN :
        TIME_CONTINUITY_EVIDENCE_INDETERMINATE;
}

static Tr2Result initialize_rtc(Stm32RuntimePlatformContext *platform)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_PeriphCLKInitTypeDef periph = {0};

    tr2_rtc_h3ec_init_stage = 1U;
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();

    osc.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    osc.LSEState = RCC_LSE_ON;
    tr2_rtc_h3ec_lse_hal_status = (uint32_t)HAL_RCC_OscConfig(&osc);
    if (tr2_rtc_h3ec_lse_hal_status != (uint32_t)HAL_OK) {
        return TR2_ERROR_UNAVAILABLE;
    }

    tr2_rtc_h3ec_init_stage = 2U;
    periph.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    periph.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    tr2_rtc_h3ec_clock_hal_status =
        (uint32_t)HAL_RCCEx_PeriphCLKConfig(&periph);
    if (tr2_rtc_h3ec_clock_hal_status != (uint32_t)HAL_OK) {
        return TR2_ERROR_UNAVAILABLE;
    }

    tr2_rtc_h3ec_init_stage = 3U;
    __HAL_RCC_RTC_ENABLE();
    __HAL_RCC_RTCAPB_CLK_ENABLE();
    __HAL_RCC_RTCAPB_CLKAM_ENABLE();

    platform->rtc.Instance = RTC;
    platform->rtc.Init.HourFormat = RTC_HOURFORMAT_24;
    platform->rtc.Init.AsynchPrediv = 127U;
    platform->rtc.Init.SynchPrediv = 255U;
    platform->rtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    platform->rtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
    platform->rtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    platform->rtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    platform->rtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;

    tr2_rtc_h3ec_rtc_hal_status = (uint32_t)HAL_RTC_Init(&platform->rtc);
    if (tr2_rtc_h3ec_rtc_hal_status != (uint32_t)HAL_OK) {
        return TR2_ERROR_UNAVAILABLE;
    }

    tr2_rtc_h3ec_init_stage = 4U;
    platform->rtc_available = true;

    /* H3e-C diagnostic snapshot after successful RTC initialization. */
    tr2_rtc_h3ec_reg_tr = RTC->TR;
    tr2_rtc_h3ec_reg_dr = RTC->DR;
    tr2_rtc_h3ec_reg_icsr = RTC->ICSR;
    tr2_rtc_h3ec_reg_bkp0r =
        HAL_RTCEx_BKUPRead(&platform->rtc, TR2_RTC_CONTINUITY_BACKUP_REGISTER);
    return TR2_OK;
}

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
    Tr2Result rtc_result;

    g_runtime_platform.boot_reset_cause = capture_reset_cause();
    g_runtime_platform.rtc_available = false;
    g_runtime_platform.initialized = true;
    rtc_result = initialize_rtc(&g_runtime_platform);
    __HAL_RCC_CLEAR_RESET_FLAGS();
    return rtc_result;
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

WallClock stm32_runtime_wall_clock(void)
{
    WallClock clock = {
        &g_runtime_platform,
        stm32_wall_clock_read,
        stm32_wall_clock_set
    };
    return clock;
}

TimeContinuityEvidenceProvider stm32_runtime_time_continuity_evidence_provider(void)
{
    TimeContinuityEvidenceProvider provider = {
        &g_runtime_platform,
        stm32_time_continuity_get
    };
    return provider;
}
