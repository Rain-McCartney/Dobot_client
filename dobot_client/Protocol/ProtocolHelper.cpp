#include "Protocol/ProtocolHelper.h"


void applyCOBS(uint8_t* frame, int frameLength)
{
    for (int i = frameLength - 2; i > 0; i--) {
        frame[i] = frame[i - 1];
    }
    // Counter to the next synchro-byte.
    uint8_t counter = 1;
    uint8_t* currentIndexToWrite = frame;
    // Pointer to the end of frame buffer.
    uint8_t* endOfFrame = frame + frameLength - 1;
    // Write synchro-byte to the end of buffer.
    *endOfFrame = ZeroPadding;
    // Go through message buffer.
    while (++frame <= endOfFrame)
    {
        // Write current counter value instead of synchro-byte.
        if (*frame == ZeroPadding)
        {
            // Write counter.
            *currentIndexToWrite = counter;
            // Change pointer to index to write.
            currentIndexToWrite += counter;
            // Reset counter.
            counter = 1;
        }
        else
        {
            counter++;
        }
    }
}

retStatusCode decodeWithCobs(uint8_t *receivedMessage, uint32_t &size)
{
    const uint8_t cobsOverHead = 2;

    if (receivedMessage == nullptr)
    {
        return STATUS_BAD_ARGUMENTS;
    }
    if (size <= 0)
    {
        return STATUS_BAD_ARGUMENTS;
    }
    const uint32_t cobsZeroPaddingOffset = size - 1;

    // Return STATUS_SYNCHRO_LOST, if there is no synchro-byte at the end.
    if (receivedMessage[cobsZeroPaddingOffset] != ZeroPadding)
    {
        return STATUS_PACKET_SYNCHRO_LOST;
    }

    // Make sure that there is no other synchro-byte than last byte
    for (int i = cobsZeroPaddingOffset - 1; i >= 0; i--)
    {
        if (receivedMessage[i] == ZeroPadding)
        {
            return STATUS_PACKET_SYNCHRO_LOST;
        }
    }
    const auto codedSize = static_cast<size_t>(size);
    uint8_t decodeBuffer[MaximumLengthInBytes] = { 0 };
    size_t decodedSize = decodeCobs(receivedMessage, codedSize, decodeBuffer);

    if ((decodedSize <= MaximumLengthInBytes) && (decodedSize <= codedSize))
    {
        size = static_cast<int>(decodedSize);
        memcpy(receivedMessage + 1, decodeBuffer, decodedSize - cobsOverHead);
        return STATUS_OK;
    }
    else
    {
        return STATUS_PACKET_SYNCHRO_LOST;
    }
}

size_t decodeCobs(const uint8_t *ptr,
                  size_t length,
                  uint8_t *dst)
{
    const uint8_t *start = dst, *end = ptr + length;
    uint8_t code = 0xFF, copy = 0;
    for (; ptr < end; copy--)
    {
        if (copy != 0)
        {
            *dst++ = *ptr++;
        }
        else
        {
            if (code != 0xFF)
            {
                *dst++ = 0;
            }
            copy = code = *ptr++;
            if (code == 0)
            {
                /* Source length too long */
                break;
            }
        }
    }
    *dst++ = 0;
    return dst - start;
}

uint32_t extractCRC32FromReceivedBuffer(const uint8_t *receivedBuffer,
                                        size_t receivedBufferLength,
                                        size_t crcLsbOffsetFromTheEndOfFrame)
{
    const uint32_t byte3 = receivedBuffer[receivedBufferLength - crcLsbOffsetFromTheEndOfFrame - 0] << 24u;
    const uint32_t byte2 = receivedBuffer[receivedBufferLength - crcLsbOffsetFromTheEndOfFrame - 1] << 16u;
    const uint32_t byte1 = receivedBuffer[receivedBufferLength - crcLsbOffsetFromTheEndOfFrame - 2] << 8u;
    const uint32_t byte0 = receivedBuffer[receivedBufferLength - crcLsbOffsetFromTheEndOfFrame - 3];
    const uint32_t extractedCRC32 = byte3 | byte2 | byte1 | byte0;
    return extractedCRC32;
}

retStatusCode validateCRC32(uint32_t extractedCRC32,
                            uint32_t calculatedCRC32)
{
    retStatusCode retStatus = STATUS_OK;

    if (extractedCRC32 != calculatedCRC32)
    {
//        qCritical() << "ValidateCRC32 - Wrong CRC received:" << QString::number(extractedCRC32) << ", the calculated one is:" << QString::number(calculatedCRC32);
        retStatus = STATUS_PACKET_WRONG_CRC;
    }

    return retStatus;
}

retStatusCode validatePacketLength(const uint8_t *receivedMessage, uint32_t receivedMessageSize)
{
    retStatusCode retStatus = STATUS_OK;
    const uint8_t packetLength = receivedMessage[PacketLengthOffsetInBytes + COBSFirstByteOffset];

    if (packetLength != (receivedMessageSize - 2))
    {
//        qWarning() << "ValidatePacketLength- The received packet length in frame: " << (int)packetLength << " does not correspond to the received frame size : " << receivedMessageFrameSize;
        retStatus = STATUS_PACKET_WRONG_SIZE;
    }
    return retStatus;
}
