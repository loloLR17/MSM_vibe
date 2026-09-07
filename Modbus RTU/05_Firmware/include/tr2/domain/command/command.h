#ifndef TR2_DOMAIN_COMMAND_H
#define TR2_DOMAIN_COMMAND_H

#include <stdbool.h>
#include <stdint.h>

#define TR2_COMMAND_TRANSACTION_ID_INVALID 0u
#define TR2_COMMAND_CONFIRM_KEY_NONE 0x0000u
#define TR2_COMMAND_CONFIRM_KEY_VALID 0xA55Au

typedef enum {
    COMMAND_CODE_NONE = 0,
    COMMAND_CODE_APPLY_CONFIGURATION = 1,
    COMMAND_CODE_SYNCHRONIZE_TIME = 2,
    COMMAND_CODE_START_ACQUISITION = 3,
    COMMAND_CODE_STOP_ACQUISITION = 4,
    COMMAND_CODE_SELFTEST = 5,
    COMMAND_CODE_ACKNOWLEDGE_FAULT = 6,
    COMMAND_CODE_REFRESH_INDICATORS = 7,
    COMMAND_CODE_ENTER_MAINTENANCE = 8,
    COMMAND_CODE_EXIT_MAINTENANCE = 9,
    COMMAND_CODE_SOFTWARE_RESET = 10,
    COMMAND_CODE_RESET_STATISTICS = 11
} CommandCode;

typedef enum {
    COMMAND_STATUS_NONE = 0,
    COMMAND_STATUS_RECEIVED = 1,
    COMMAND_STATUS_ACCEPTED = 2,
    COMMAND_STATUS_RUNNING = 3,
    COMMAND_STATUS_SUCCESS = 4,
    COMMAND_STATUS_REFUSED = 5,
    COMMAND_STATUS_FAILED = 6,
    COMMAND_STATUS_UNKNOWN = 7,
    COMMAND_STATUS_NOT_ALLOWED = 8
} CommandStatus;

typedef enum {
    COMMAND_RESULT_SUCCESS = 0,
    COMMAND_RESULT_UNKNOWN_COMMAND = 1,
    COMMAND_RESULT_INVALID_PARAMETER = 2,
    COMMAND_RESULT_INCOMPATIBLE_STATE = 3,
    COMMAND_RESULT_INVALID_CONFIGURATION = 4,
    COMMAND_RESULT_ACQUISITION_RUNNING = 5,
    COMMAND_RESULT_STORAGE_ABSENT = 6,
    COMMAND_RESULT_INSUFFICIENT_MEMORY = 7,
    COMMAND_RESULT_CRITICAL_FAULT_ACTIVE = 8,
    COMMAND_RESULT_CONFIRMATION_MISSING = 9,
    COMMAND_RESULT_INTERNAL_TIMEOUT = 10,
    COMMAND_RESULT_SELFTEST_FAILED = 11,
    COMMAND_RESULT_CLOCK_UNAVAILABLE = 12,
    COMMAND_RESULT_COMMAND_ALREADY_RUNNING = 13,
    COMMAND_RESULT_INVALID_TRANSACTION_ID = 14,
    COMMAND_RESULT_NOT_CANCELLABLE = 15,
    COMMAND_RESULT_FAULT_NOT_ACKNOWLEDGEABLE = 16,
    COMMAND_RESULT_MAINTENANCE_REQUIRED = 17,
    COMMAND_RESULT_MAINTENANCE_ACTIVE = 18,
    COMMAND_RESULT_PREPARED_TIME_ABSENT = 19,
    COMMAND_RESULT_PREPARED_CONFIGURATION_INCOMPLETE = 20,
    COMMAND_RESULT_ACQUISITION_NOT_ACTIVE = 21,
    COMMAND_RESULT_ACTIVE_CONFIGURATION_INVALID = 22
} CommandResultCode;

typedef enum {
    COMMAND_LIFECYCLE_RESERVED = 0,
    COMMAND_LIFECYCLE_STARTED = 1,
    COMMAND_LIFECYCLE_COMPLETED = 2
} CommandLifecycleState;

typedef enum {
    COMMAND_RECONCILIATION_TERMINAL_EFFECT_PROVEN = 0,
    COMMAND_RECONCILIATION_ABSENCE_PROVEN = 1,
    COMMAND_RECONCILIATION_INDETERMINATE = 2
} CommandReconciliationOutcome;

typedef struct {
    uint16_t command_code;
    uint16_t param1;
    uint16_t param2;
    uint32_t param3;
    uint16_t confirm_key;
} CommandRequestIdentity;

typedef struct {
    uint16_t transaction_id;
    CommandRequestIdentity identity;
} CommandRequest;

typedef struct {
    uint16_t status;
    uint16_t result_code;
    uint16_t result_detail;
} CommandFinalResult;

typedef struct {
    bool available;
    uint32_t value;
} CommandTerminalTimestamp;

typedef struct {
    bool present;
    uint16_t command_code;
    uint16_t transaction_id;
    CommandFinalResult final_result;
    CommandTerminalTimestamp terminal_timestamp;
} LastCommandSnapshot;

typedef struct {
    uint32_t generation;
    uint16_t active_command_code;
    uint16_t active_transaction_id;
    uint16_t status;
    uint16_t result_code;
    uint16_t result_detail;
    uint16_t engine_flags;
    LastCommandSnapshot last;
} CommandSnapshot;

bool command_transaction_id_is_valid(uint16_t transaction_id);
bool command_request_identity_equal(const CommandRequestIdentity *left,
                                    const CommandRequestIdentity *right);
bool command_status_is_final(uint16_t status);

#endif
