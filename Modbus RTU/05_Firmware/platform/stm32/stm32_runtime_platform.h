#ifndef TR2_STM32_RUNTIME_PLATFORM_H
#define TR2_STM32_RUNTIME_PLATFORM_H

#include "tr2/common/result.h"
#include "tr2/platform/monotonic_clock.h"
#include "tr2/platform/reset_cause_provider.h"
#include "tr2/platform/time_continuity_evidence.h"
#include "tr2/platform/wall_clock.h"

Tr2Result stm32_runtime_platform_init(void);
MonotonicClock stm32_runtime_monotonic_clock(void);
ResetCauseProvider stm32_runtime_reset_cause_provider(void);
WallClock stm32_runtime_wall_clock(void);
TimeContinuityEvidenceProvider stm32_runtime_time_continuity_evidence_provider(void);

#endif
