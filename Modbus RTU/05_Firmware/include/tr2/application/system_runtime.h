#ifndef TR2_APPLICATION_SYSTEM_RUNTIME_H
#define TR2_APPLICATION_SYSTEM_RUNTIME_H

#include <stdbool.h>
#include "tr2/common/result.h"
#include "tr2/platform/monotonic_clock.h"
#include "tr2/platform/persistent_media.h"
#include "tr2/platform/reset_cause_provider.h"
#include "tr2/platform/wall_clock.h"

typedef struct {
    ResetCause reset_cause;
} BootContext;

typedef struct {
    const MonotonicClock *monotonic_clock;
    const WallClock *wall_clock;
    const ResetCauseProvider *reset_cause_provider;
    const PersistentMedia *persistent_media;
} SystemRuntimeDependencies;

typedef struct {
    SystemRuntimeDependencies deps;
    BootContext boot_context;
    bool initialized;
    bool system_ready_for_modbus;
} SystemRuntime;

Tr2Result system_runtime_init(SystemRuntime *runtime, const SystemRuntimeDependencies *deps);
Tr2Result system_runtime_boot(SystemRuntime *runtime);
const BootContext *system_runtime_boot_context(const SystemRuntime *runtime);
bool system_runtime_is_ready_for_modbus(const SystemRuntime *runtime);

#endif
