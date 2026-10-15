#ifndef TR2_COMMON_SHA256_H
#define TR2_COMMON_SHA256_H
#include <stddef.h>
#include <stdint.h>
#include "tr2/common/result.h"
#include "tr2/common/cooperative_work.h"
/* Integrity/provenance digest (FIPS 180-4), not authentication or certification. */
Tr2Result tr2_sha256_bytes(const uint8_t *data, size_t size, uint8_t digest[32]);
Tr2Result tr2_sha256_bytes_cooperative(const uint8_t *,size_t,uint8_t [32],Tr2CooperativeWork *);
#endif
