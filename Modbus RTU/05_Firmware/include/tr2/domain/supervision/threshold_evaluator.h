#ifndef TR2_DOMAIN_SUPERVISION_THRESHOLD_EVALUATOR_H
#define TR2_DOMAIN_SUPERVISION_THRESHOLD_EVALUATOR_H

#include "tr2/common/result.h"
#include "tr2/domain/configuration/configuration.h"
#include "tr2/domain/supervision/indicator_calculator.h"

typedef enum {
    SUPERVISION_THRESHOLD_BELOW = 0,
    SUPERVISION_THRESHOLD_EQUAL,
    SUPERVISION_THRESHOLD_ABOVE
} SupervisionThresholdRelation;

typedef struct {
    SupervisionThresholdRelation rms_warning;
    SupervisionThresholdRelation rms_alarm;
    SupervisionThresholdRelation peak_warning;
    SupervisionThresholdRelation peak_alarm;
} SupervisionThresholdMetricFacts;

typedef struct {
    SupervisionThresholdMetricFacts global;
    SupervisionThresholdMetricFacts x;
    SupervisionThresholdMetricFacts y;
    SupervisionThresholdMetricFacts z;
} SupervisionThresholdFacts;

Tr2Result supervision_thresholds_compare(
    const VibrationIndicators *indicators,
    const ConfigurationPayload *configuration,
    SupervisionThresholdFacts *out_facts);

#endif
