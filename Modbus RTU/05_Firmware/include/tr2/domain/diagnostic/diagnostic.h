#ifndef TR2_DOMAIN_DIAGNOSTIC_H
#define TR2_DOMAIN_DIAGNOSTIC_H

#include <stdbool.h>
#include <stdint.h>

#define TR2_DIAGNOSTIC_ACK_TRACKING_CAPACITY 16u

typedef enum {
    DIAGNOSTIC_HEALTH_OK = 0,
    DIAGNOSTIC_HEALTH_WARNING,
    DIAGNOSTIC_HEALTH_DEGRADED,
    DIAGNOSTIC_HEALTH_CRITICAL
} DiagnosticHealth;

typedef enum {
    DIAGNOSTIC_SELFTEST_NEVER_RUN = 0,
    DIAGNOSTIC_SELFTEST_RUNNING,
    DIAGNOSTIC_SELFTEST_PASSED,
    DIAGNOSTIC_SELFTEST_FAILED
} DiagnosticSelfTestState;

typedef struct {
    bool sensor_fault;
    bool acquisition_fault;
    bool memory_fault;
    bool storage_fault;
    bool time_fault;
    bool configuration_fault;
    bool firmware_fault;
    bool overcurrent_fault;
    bool temperature_out_of_range;
    bool internal_communication_fault;
} DiagnosticActiveConditions;

typedef struct {
    bool present;
    uint16_t code;
    bool timestamp_available;
    uint32_t timestamp;
} DiagnosticLastFault;

typedef struct {
    uint16_t code;
    bool acknowledgeable;
} DiagnosticActiveFault;

typedef struct {
    bool active;
    uint16_t code;
    bool acknowledgeable;
    bool acknowledged;
} DiagnosticFaultAcknowledgement;

typedef struct {
    DiagnosticSelfTestState state;
    uint16_t result_code;
    uint16_t detail;
} DiagnosticSelfTestFacts;

typedef struct {
    DiagnosticHealth health;
    DiagnosticActiveConditions active_conditions;
    DiagnosticLastFault last_fault;
    DiagnosticSelfTestFacts selftest;
    bool internal_temperature_available;
    int16_t internal_temp_dC;
    bool supply_voltage_available;
    uint16_t supply_voltage_mV;
} DiagnosticFacts;

typedef struct {
    uint32_t generation;
    DiagnosticFacts facts;
} DiagnosticSnapshot;

#endif
