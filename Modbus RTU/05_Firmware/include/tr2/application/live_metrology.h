#ifndef TR2_LIVE_METROLOGY_H
#define TR2_LIVE_METROLOGY_H
#include "tr2/application/live_configuration.h"
#include "tr2/modbus/projection.h"

typedef struct {
    bool warning, alarm, pending;
    uint64_t candidate_ms;
} LiveMetricAlarm;
/* One owner, non-reentrant. No heap, media or hardware access. A caller supplies
 * observed sample times (not dequeue time or a fabricated UTC). Reboot init only. */
typedef struct {
    ActiveConfigurationSnapshot active;
    AcquisitionWindow window;
    SupervisionSnapshot value;
    LiveMetricAlarm alarms[4];
    uint64_t run_id, next_index, last_ms, last_read_ms, next_window_ms;
    uint32_t sequence, exceed_count, alarm_count;
    bool boot, authority, running, has_value, value_valid, error, calc_error, latched;
    bool window_active, time_seen, read_time_seen;
    void *stop_context;
    void (*safe_stop)(void *);
} LiveMetrology;
void live_metrology_init(LiveMetrology *);
Tr2Result live_metrology_start(LiveMetrology *, uint64_t run,
    const ActiveConfigurationSnapshot *);
void live_metrology_stop(LiveMetrology *);
void live_metrology_invalidate(LiveMetrology *, bool calculation_error, bool authority_lost);
/* index is the exact XYZ position from R3-A, including samples between windows.
 * UTC is an observed per-sample instant; >UINT32_MAX becomes unavailable. */
Tr2Result live_metrology_sample(LiveMetrology *, uint64_t run, uint32_t config_id,
    uint32_t generation, uint64_t index, const int16_t xyz[3], uint64_t mono_ms,
    bool utc_valid, uint64_t utc_s);
Tr2Result live_metrology_project_b3(LiveMetrology *, uint64_t now_ms, ModbusBlock3Image *);
Tr2Result live_metrology_ack(LiveMetrology *, uint64_t now_ms);
#endif
