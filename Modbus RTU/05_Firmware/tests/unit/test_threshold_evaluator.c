#include <assert.h>
#include <string.h>

#include "tr2/application/supervision_service.h"
#include "tr2/domain/supervision/threshold_evaluator.h"

static void test_direct_comparisons(void)
{
    ConfigurationPayload configuration;
    VibrationIndicators indicators;
    SupervisionThresholdFacts facts;

    memset(&configuration, 0, sizeof(configuration));
    memset(&indicators, 0, sizeof(indicators));

    configuration.rms_warn_threshold_mg = 100u;
    configuration.rms_alarm_threshold_mg = 200u;
    configuration.peak_warn_threshold_mg = 300u;
    configuration.peak_alarm_threshold_mg = 400u;

    indicators.rms_global_mg = 100u;
    indicators.peak_global_mg = 401u;
    indicators.rms_x_mg = 99u;
    indicators.peak_x_mg = 300u;
    indicators.rms_y_mg = 200u;
    indicators.peak_y_mg = 299u;
    indicators.rms_z_mg = 201u;
    indicators.peak_z_mg = 400u;

    assert(supervision_thresholds_compare(&indicators, &configuration, &facts) == TR2_OK);

    assert(facts.global.rms_warning == SUPERVISION_THRESHOLD_EQUAL);
    assert(facts.global.rms_alarm == SUPERVISION_THRESHOLD_BELOW);
    assert(facts.global.peak_warning == SUPERVISION_THRESHOLD_ABOVE);
    assert(facts.global.peak_alarm == SUPERVISION_THRESHOLD_ABOVE);

    assert(facts.x.rms_warning == SUPERVISION_THRESHOLD_BELOW);
    assert(facts.x.rms_alarm == SUPERVISION_THRESHOLD_BELOW);
    assert(facts.x.peak_warning == SUPERVISION_THRESHOLD_EQUAL);
    assert(facts.x.peak_alarm == SUPERVISION_THRESHOLD_BELOW);

    assert(facts.y.rms_warning == SUPERVISION_THRESHOLD_ABOVE);
    assert(facts.y.rms_alarm == SUPERVISION_THRESHOLD_EQUAL);
    assert(facts.y.peak_warning == SUPERVISION_THRESHOLD_BELOW);
    assert(facts.y.peak_alarm == SUPERVISION_THRESHOLD_BELOW);

    assert(facts.z.rms_warning == SUPERVISION_THRESHOLD_ABOVE);
    assert(facts.z.rms_alarm == SUPERVISION_THRESHOLD_ABOVE);
    assert(facts.z.peak_warning == SUPERVISION_THRESHOLD_ABOVE);
    assert(facts.z.peak_alarm == SUPERVISION_THRESHOLD_EQUAL);

    assert(supervision_thresholds_compare(NULL, &configuration, &facts) == TR2_ERROR_INVALID_ARGUMENT);
    assert(supervision_thresholds_compare(&indicators, NULL, &facts) == TR2_ERROR_INVALID_ARGUMENT);
    assert(supervision_thresholds_compare(&indicators, &configuration, NULL) == TR2_ERROR_INVALID_ARGUMENT);
}

static void test_snapshot_uses_frozen_window_thresholds(void)
{
    SupervisionService service;
    SupervisionSnapshot snapshot;
    AcquisitionWindow window;

    memset(&window, 0, sizeof(window));
    window.configuration.generation = 7u;
    window.configuration.config_id = 42u;
    window.configuration.revision_counter = 9u;
    window.configuration.payload.window_size_samples = 1u;
    window.configuration.payload.rms_warn_threshold_mg = 100u;
    window.configuration.payload.rms_alarm_threshold_mg = 150u;
    window.configuration.payload.peak_warn_threshold_mg = 90u;
    window.configuration.payload.peak_alarm_threshold_mg = 100u;
    window.start_monotonic_ms = 1000u;
    window.end_monotonic_ms = 1100u;
    window.acquired_sample_count = 1u;
    window.valid_sample_count = 1u;
    window.sum_square_x_mg2 = 10000u;
    window.sum_square_vector_mg2 = 10000u;
    window.peak_abs_x_mg = 100u;
    window.peak_vector_square_mg2 = 10000u;
    window.complete = true;

    assert(supervision_service_init(&service) == TR2_OK);
    assert(supervision_service_publish_window(&service, &window) == TR2_OK);
    assert(supervision_service_snapshot(&service, &snapshot));

    assert(snapshot.configuration.generation == 7u);
    assert(snapshot.configuration.payload.rms_warn_threshold_mg == 100u);
    assert(snapshot.threshold_facts_available);
    assert(snapshot.threshold_facts.global.rms_warning == SUPERVISION_THRESHOLD_EQUAL);
    assert(snapshot.threshold_facts.global.rms_alarm == SUPERVISION_THRESHOLD_BELOW);
    assert(snapshot.threshold_facts.global.peak_warning == SUPERVISION_THRESHOLD_ABOVE);
    assert(snapshot.threshold_facts.global.peak_alarm == SUPERVISION_THRESHOLD_EQUAL);

    /* Equality remains a fact; P5-F does not turn it into B3_EXCEED or alarm state. */
    assert(snapshot.threshold_facts.x.rms_warning == SUPERVISION_THRESHOLD_EQUAL);
    assert(snapshot.threshold_facts.x.peak_alarm == SUPERVISION_THRESHOLD_EQUAL);
}

int main(void)
{
    test_direct_comparisons();
    test_snapshot_uses_frozen_window_thresholds();
    return 0;
}
