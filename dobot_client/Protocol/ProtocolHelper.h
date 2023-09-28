
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "Errors.hpp"

const uint8_t ZeroPadding = 0x00;

const uint32_t normalMessageTimeout = 1000;

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
