#ifndef TR2_IIS3DWB_DIAG_WINDOW_H
#define TR2_IIS3DWB_DIAG_WINDOW_H

#include "tr2/application/supervision_service.h"
#include "tr2/modbus/projection.h"
#include "tr2/platform/vibration_source.h"

/* Diagnostic-only profile; not an active configuration or a P8 campaign. */
#define TR2_IIS3DWB_DIAG_WINDOW_SAMPLES 4096U

typedef struct {
    AcquisitionWindow window;
    SupervisionService supervision;
    SupervisionSnapshot snapshot;
    ModbusBlock3Image image;
    bool image_available;
} Iis3dwbDiagWindow;

Tr2Result iis3dwb_diag_window_begin(Iis3dwbDiagWindow *diag, MonotonicTimeMs now);
Tr2Result iis3dwb_diag_window_append(Iis3dwbDiagWindow *diag,
                                    const VibrationSample *sample);
Tr2Result iis3dwb_diag_window_publish(Iis3dwbDiagWindow *diag, MonotonicTimeMs now);

#endif
