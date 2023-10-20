
#include "DataHelper.h"

uint32_t createUint32FromPayload(const uint8_t *payload, const uint8_t offset)
{
    uint32_t value = (payload[offset + 0] << 0)
            | (payload[offset + 1] << 8)
            | (payload[offset + 2] << 16)
            | (payload[offset + 3] << 24);

    return value;
}

void createPayloadFromUint32(const uint32_t value,
                             const uint8_t offset,
                             uint8_t *payload)
{
    payload[offset + 0] = (value >> 0)  & 0xFF;
    payload[offset + 1] = (value >> 8)  & 0xFF;
    payload[offset + 2] = (value >> 16) & 0xFF;
    payload[offset + 3] = (value >> 24) & 0xFF;
}
