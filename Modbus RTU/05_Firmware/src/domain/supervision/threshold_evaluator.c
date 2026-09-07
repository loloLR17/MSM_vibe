#include "tr2/domain/supervision/threshold_evaluator.h"

#include <stddef.h>

static SupervisionThresholdRelation compare_u32_u16(uint32_t value,
                                                    uint16_t threshold)
{
    if (value < (uint32_t)threshold) {
        return SUPERVISION_THRESHOLD_BELOW;
    }
    if (value > (uint32_t)threshold) {
        return SUPERVISION_THRESHOLD_ABOVE;
    }
    return SUPERVISION_THRESHOLD_EQUAL;
}

static SupervisionThresholdMetricFacts compare_metric(uint32_t rms_mg,
                                                      uint32_t peak_mg,
                                                      const ConfigurationPayload *configuration)
{
    SupervisionThresholdMetricFacts facts;

    facts.rms_warning = compare_u32_u16(rms_mg, configuration->rms_warn_threshold_mg);
    facts.rms_alarm = compare_u32_u16(rms_mg, configuration->rms_alarm_threshold_mg);
    facts.peak_warning = compare_u32_u16(peak_mg, configuration->peak_warn_threshold_mg);
    facts.peak_alarm = compare_u32_u16(peak_mg, configuration->peak_alarm_threshold_mg);
    return facts;
}

Tr2Result supervision_thresholds_compare(
    const VibrationIndicators *indicators,
    const ConfigurationPayload *configuration,
    SupervisionThresholdFacts *out_facts)
{
    if (indicators == NULL || configuration == NULL || out_facts == NULL) {
        return TR2_ERROR_INVALID_ARGUMENT;
    }

    out_facts->global = compare_metric(indicators->rms_global_mg,
                                       indicators->peak_global_mg,
                                       configuration);
    out_facts->x = compare_metric(indicators->rms_x_mg,
                                  indicators->peak_x_mg,
                                  configuration);
    out_facts->y = compare_metric(indicators->rms_y_mg,
                                  indicators->peak_y_mg,
                                  configuration);
    out_facts->z = compare_metric(indicators->rms_z_mg,
                                  indicators->peak_z_mg,
                                  configuration);
    return TR2_OK;
}
