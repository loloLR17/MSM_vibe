#ifndef CLICK_POLICY_H
#define CLICK_POLICY_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#define CLICK_ARM UINT32_C(0xC118A55A)
bool click_mode_allowed(uint32_t command, uint32_t arm, bool de_low);
size_t click_raw_reply(const uint8_t *input, size_t length, const uint32_t uid[3],
                      uint8_t *output, size_t capacity);
#endif
