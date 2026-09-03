#ifndef TR2_PLATFORM_RESET_CAUSE_PROVIDER_H
#define TR2_PLATFORM_RESET_CAUSE_PROVIDER_H

typedef enum {
    RESET_CAUSE_UNKNOWN = 0,
    RESET_CAUSE_POWER_ON,
    RESET_CAUSE_SOFTWARE,
    RESET_CAUSE_WATCHDOG,
    RESET_CAUSE_EXTERNAL,
    RESET_CAUSE_BROWNOUT
} ResetCause;

typedef struct {
    void *context;
    ResetCause (*get)(void *context);
} ResetCauseProvider;

#endif
