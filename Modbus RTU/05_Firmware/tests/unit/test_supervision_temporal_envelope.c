#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>

#include "tr2/application/supervision_service.h"

static MonotonicTimeMs fake_now_ms(void *context)
{
    return *(MonotonicTimeMs *)context;
}

static WallClockReadResult fake_wall_read(void *context,
                                          Tr2CivilTimestamp *timestamp)
{
    *timestamp = *(Tr2CivilTimestamp *)context;
    return WALL_CLOCK_OK;
}

static AcquisitionWindow make_window(MonotonicTimeMs start_ms,
                                     MonotonicTimeMs end_ms)
{
    AcquisitionWindow window;

    memset(&window, 0, sizeof(window));
    window.configuration.generation = 1u;
    window.configuration.config_id = 10u;
    window.configuration.revision_counter = 2u;
    window.configuration.payload.window_size_samples = 1u;
    window.start_monotonic_ms = start_ms;
    window.end_monotonic_ms = end_ms;
    window.acquired_sample_count = 1u;
    window.valid_sample_count = 1u;
    window.sum_square_x_mg2 = 9u;
    window.sum_square_y_mg2 = 16u;
    window.sum_square_vector_mg2 = 25u;
    window.peak_abs_x_mg = 3u;
    window.peak_abs_y_mg = 4u;
    window.peak_vector_square_mg2 = 25u;
    window.complete = true;
    return window;
}

int main(void)
{
    SupervisionService service;
    SupervisionSnapshot snapshot;
    TimeService time_service;
    TimeRecoveryContext recovery;
    AcquisitionWindow window;
    MonotonicTimeMs now_ms = 1500u;
    Tr2CivilTimestamp civil_time = 123456u;
    MonotonicClock monotonic_clock = { &now_ms, fake_now_ms };
    WallClock wall_clock = { &civil_time, fake_wall_read, NULL };

    assert(time_service_init(&time_service, &wall_clock) == TR2_OK);
    recovery.civil_time_usable = true;
    recovery.continuity = TIME_CONTINUITY_PROVEN;
    recovery.last_sync_history = time_last_sync_history_valid(123000u, 1u);
    assert(time_service_apply_recovery_context(&time_service, &recovery) == TR2_OK);

    assert(supervision_service_init(&service) == TR2_OK);
    assert(supervision_service_bind_temporal_dependencies(&service,
                                                          &monotonic_clock,
                                                          &time_service) == TR2_OK);

    window = make_window(1000u, 1200u);
    assert(supervision_service_publish_window(&service, &window) == TR2_OK);
    assert(supervision_service_snapshot(&service, &snapshot));
    assert(snapshot.value_age_available);
    assert(snapshot.value_age_ms == 300u);
    assert(snapshot.civil_timestamp_available);
    assert(snapshot.civil_timestamp == 123456u);

    now_ms = 1700u;
    assert(supervision_service_snapshot(&service, &snapshot));
    assert(snapshot.value_age_available);
    assert(snapshot.value_age_ms == 500u);

    now_ms = 1100u;
    assert(supervision_service_snapshot(&service, &snapshot));
    assert(!snapshot.value_age_available);
    assert(snapshot.value_age_ms == 0u);

    now_ms = (MonotonicTimeMs)UINT32_MAX + 5000u;
    window = make_window(0u, 1u);
    assert(supervision_service_publish_window(&service, &window) == TR2_OK);
    assert(supervision_service_snapshot(&service, &snapshot));
    assert(snapshot.value_age_available);
    assert(snapshot.value_age_ms == UINT32_MAX);

    recovery.civil_time_usable = false;
    recovery.continuity = TIME_CONTINUITY_INDETERMINATE;
    recovery.last_sync_history = time_last_sync_history_none();
    assert(time_service_apply_recovery_context(&time_service, &recovery) == TR2_OK);
    now_ms = 2500u;
    window = make_window(2000u, 2200u);
    assert(supervision_service_publish_window(&service, &window) == TR2_OK);
    assert(supervision_service_snapshot(&service, &snapshot));
    assert(!snapshot.civil_timestamp_available);
    assert(snapshot.civil_timestamp == 0u);

    return 0;
}
