#ifndef TR2_APPLICATION_ACQUISITION_SERVICE_H
#define TR2_APPLICATION_ACQUISITION_SERVICE_H

#include <stdbool.h>

#include "tr2/application/configuration_service.h"
#include "tr2/common/result.h"
#include "tr2/domain/acquisition/acquisition.h"
#include "tr2/platform/monotonic_clock.h"
#include "tr2/platform/vibration_source.h"

typedef struct {
    ConfigurationService *configuration_service;
    const MonotonicClock *monotonic_clock;
    VibrationSource *vibration_source;
    bool initialized;
    bool window_active;
    bool source_started;
    AcquisitionWindow current_window;
} AcquisitionService;

Tr2Result acquisition_service_init(AcquisitionService *service,
                                   ConfigurationService *configuration_service,
                                   const MonotonicClock *monotonic_clock,
                                   VibrationSource *vibration_source);

bool acquisition_service_is_initialized(const AcquisitionService *service);
bool acquisition_service_window_active(const AcquisitionService *service);

Tr2Result acquisition_service_begin_window(AcquisitionService *service);
Tr2Result acquisition_service_read_sample(AcquisitionService *service,
                                          VibrationSample *out_sample);
Tr2Result acquisition_service_end_window(AcquisitionService *service,
                                         AcquisitionWindow *out_window);

#endif
