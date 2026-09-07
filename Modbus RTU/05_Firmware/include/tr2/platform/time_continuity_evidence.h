#ifndef TR2_PLATFORM_TIME_CONTINUITY_EVIDENCE_H
#define TR2_PLATFORM_TIME_CONTINUITY_EVIDENCE_H

typedef enum {
    TIME_CONTINUITY_EVIDENCE_INDETERMINATE = 0,
    TIME_CONTINUITY_EVIDENCE_PROVEN,
    TIME_CONTINUITY_EVIDENCE_BROKEN
} TimeContinuityEvidence;

typedef struct {
    void *context;
    TimeContinuityEvidence (*get)(void *context);
} TimeContinuityEvidenceProvider;

#endif
