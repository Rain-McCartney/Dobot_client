
#pragma once

#include <cstdint>

uint32_t createUint32FromPayload(const uint8_t* payload,
                                 const uint8_t offset);

void createPayloadFromUint32(const uint32_t value,
                             const uint8_t offset,
                             uint8_t* payload);

