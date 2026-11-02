#ifndef TR2_BUDGET_DIAGNOSTIC_H
#define TR2_BUDGET_DIAGNOSTIC_H
#include <stdint.h>
/* Diagnostic classification only. The original fail-closed OR guard remains
 * at the call site, with identical thresholds and strict > comparisons. */
static inline uint32_t r3_budget_failure_bits(uint32_t service,uint32_t work,
                                             uint32_t gap,uint32_t sd)
{
 return (service>2500u*160u?1u:0u)|(work>1000u*160u?2u:0u)|
        (gap>5000u*160u?4u:0u)|(sd>1000u*160u?8u:0u);
}
#endif
