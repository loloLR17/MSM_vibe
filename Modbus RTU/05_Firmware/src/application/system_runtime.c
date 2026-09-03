#include "tr2/application/system_runtime.h"

static bool dependencies_are_valid(const SystemRuntimeDependencies *deps)
{
    return deps != NULL &&
           deps->monotonic_clock != NULL &&
           deps->monotonic_clock->now_ms != NULL &&
           deps->wall_clock != NULL &&
           deps->wall_clock->read != NULL &&
           deps->wall_clock->set != NULL &&
           deps->reset_cause_provider != NULL &&
           deps->reset_cause_provider->get != NULL &&
           deps->persistent_media != NULL &&
           deps->persistent_media->read != NULL &&
           deps->persistent_media->write != NULL &&
           deps->persistent_media->commit != NULL;
}

Tr2Result system_runtime_init(SystemRuntime *runtime, const SystemRuntimeDependencies *deps)
{
    if (runtime == NULL || !dependencies_are_valid(deps)) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    runtime->deps = *deps;
    runtime->boot_context.reset_cause = RESET_CAUSE_UNKNOWN;
    runtime->initialized = true;
    runtime->system_ready_for_modbus = false;

    return TR2_OK;
}

Tr2Result system_runtime_boot(SystemRuntime *runtime)
{
    if (runtime == NULL || !runtime->initialized) {
        return TR2_ERROR_INVALID_STATE;
    }

    runtime->system_ready_for_modbus = false;

    /* P0 covers only G0/G1 structurally. Later prototypes extend this
       function through the frozen G2..G10 recovery sequence. */
    (void)runtime->deps.monotonic_clock->now_ms(runtime->deps.monotonic_clock->context);
    runtime->boot_context.reset_cause =
        runtime->deps.reset_cause_provider->get(runtime->deps.reset_cause_provider->context);

    /* P0 has no recovered domain authorities yet. Reaching this point is
       therefore the complete P0 publication barrier. */
    runtime->system_ready_for_modbus = true;

    return TR2_OK;
}

const BootContext *system_runtime_boot_context(const SystemRuntime *runtime)
{
    if (runtime == NULL || !runtime->initialized) {
        return NULL;
    }

    return &runtime->boot_context;
}

bool system_runtime_is_ready_for_modbus(const SystemRuntime *runtime)
{
    return runtime != NULL && runtime->initialized && runtime->system_ready_for_modbus;
}
