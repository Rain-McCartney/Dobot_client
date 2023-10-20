
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "Errors.hpp"

const uint8_t ZeroPadding = 0x00;

/*****************************************************************************
 * \brief offset reserved for the first COBS byte added to the frame
 ******************************************************************************/
const int COBSFirstByteOffset = 1;

/*****************************************************************************
 * \brief the size of the circular buffer
 ******************************************************************************/
const unsigned circularBufferSize = 1024;

const uint32_t normalMessageTimeout = 3000;

typedef enum
{
    MSG_CODE_PCM_SET_MCU_PING           = 3000, ///< The check ping MCU command

} MessageCodes;


typedef enum
{
    MinimumLengthInByte  = 10,
    MaximumPayloadLength = 64,
    MaximumLengthInBytes = 74

} FieldAndFrameSizes;

typedef enum
{
    PacketLengthOffsetInBytes    = 0,
    SequenceNumberOffsetInBytes  = 1,
    CommunicationIDOffsetInBytes = 3,
    MessageCodeOffsetInBytes     = 4,
    PayloadOffsetInBytes         = 6

} MessageOffsetsEnum;

/*****************************************************************************
 * \brief Apply COBS algorithm on Message frame
 * \param[in/out] frame
 * \param[in] frameLength
 ******************************************************************************/
void applyCOBS(uint8_t* frame, int frameLength);
retStatusCode decodeWithCobs(uint8_t* receivedMessage, uint32_t &size);
size_t decodeCobs(const uint8_t* ptr, size_t length, uint8_t* dst);

/*****************************************************************************
 * \brief Get the CRC32 value from the given message frame
 * \param[in] receivedBuffer - given message frame
 * \param[in] receivedBufferLength - given message frame size
 * \return uint32_t CRC32 sum
 ******************************************************************************/
uint32_t extractCRC32FromReceivedBuffer(const uint8_t* receivedBuffer,
                                        size_t receivedBufferLength,
                                        size_t crcLsbOffsetFromTheEndOfFrame = 2);

/*****************************************************************************
 * \brief A method that compares two CRC32
 * \param[in] extractedCRC32
 * \param[in] calculatedCRC32
 *
 * \retval ::STATUS_SERIAL_OK
 * \retval ::STATUS_WRONG_CRC
 ******************************************************************************/
retStatusCode validateCRC32(uint32_t extractedCRC32,
                            uint32_t calculatedCRC32);

/*****************************************************************************
 * \brief A method that compares the packetLength field in the frame and the actual frame size
 * \param[in] receivedMessageFrame
 * \param[in] receivedMessageFrameSize
 *
 * \retval ::STATUS_SERIAL_OK
 * \retval ::STATUS_WRONG_PACKET_LENGTH_RECEIVED
 ******************************************************************************/
retStatusCode validatePacketLength(const uint8_t* receivedMessage,
                                   uint32_t receivedMessageSize);

