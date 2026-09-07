#ifndef TR2_DOMAIN_BOOT_BOOT_INTENT_H
#define TR2_DOMAIN_BOOT_BOOT_INTENT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
    BOOT_INTENT_NONE = 0,
    BOOT_INTENT_SOFTWARE_RESET = 1
} BootIntentKind;

typedef struct {
    BootIntentKind kind;
    uint16_t transaction_id;
} BootIntent;

static inline BootIntent boot_intent_none(void)
{
    BootIntent intent = {BOOT_INTENT_NONE, 0u};
    return intent;
}

static inline BootIntent boot_intent_software_reset(uint16_t transaction_id)
{
    BootIntent intent = {BOOT_INTENT_SOFTWARE_RESET, transaction_id};
    return intent;
}

static inline bool boot_intent_is_valid(const BootIntent *intent)
{
    if (intent == NULL) {
        return false;
    }
    if (intent->kind == BOOT_INTENT_NONE) {
        return intent->transaction_id == 0u;
    }
    return intent->kind == BOOT_INTENT_SOFTWARE_RESET &&
           intent->transaction_id >= 1u;
}

#endif
