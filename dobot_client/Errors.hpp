/**
* \file Errors.hpp
*
* \copyright Copyright (c) Radio Gigabit LLC 2023.
* All rights reserved. This software product is copyrighted and is property of
* Radio Gigabit LLC and cannot be copied or used without permission
*
* \brief The Errors file.
*
* \authors Andrey Aleksandrov
*
* \date 2023.04.22
*/

#pragma once

typedef int retStatusCode;

/*****************************************************************************
 * \brief return statuses enumeration
 ******************************************************************************/
typedef enum {
    // Common system errors
    STATUS_OK                         = 0,    ///< There is no any error.
    STATUS_ERROR                      = 1,    ///< Internal error.
    STATUS_BAD_ARGUMENTS              = 2,    ///< The incoming arguments are bad/null.
    STATUS_BAD_DEVICE_ID              = 3,    ///< The device id is bad/unknown.
    STATUS_BAD_CRC                    = 4,    ///< The invalid CRC32 value


    // Common errors for devices
    STATUS_REG_NUM_IS_OUT_OF_RANGE =
        100,    ///< The register number is out of range.

    // errors for I2C
    STATUS_I2C_ERROR   = 110,
    STATUS_I2C_BUSY    = 111,
    STATUS_I2C_TIMEOUT = 112,

    // errors for SPI
    STATUS_SPI_ERROR   = 120,
    STATUS_SPI_BUSY    = 121,
    STATUS_SPI_TIMEOUT = 122,

    // errors for TX
    // STATUS_ERORR_TX_1            = 200,
    // STATUS_ERORR_TX_2            = 201,
    // ...

    // errors for RX
    // STATUS_ERORR_RX_1            = 300,
    // STATUS_ERORR_RX_2            = 301,
    // ...

    // errors for PCM
    // STATUS_ERORR_PCM_1           = 400,
    // STATUS_ERORR_PCM_2           = 401,
    // ...

    STATUS_FW_UPGRADE_IN_PROGRESS                            = 400,
    STATUS_FW_UPGRADE_NOT_IN_PROGRESS                        = 401,
    STATUS_FW_UPGRADE_IN_PENDING_STATE                       = 402,
    STATUS_FW_UPGRADE_NOT_IN_PENDING_STATE                   = 403,
    STATUS_FW_UPGRADE_FW_SIZE_INVALID                        = 404,
    STATUS_FW_UPGRADE_SEGMENT_SIZE_INVALID                   = 405,
    STATUS_FW_UPGRADE_SEGMENT_ID_INVALID                     = 406,
    STATUS_FW_UPGRADE_OVERSIZED_FW_IMAGE                     = 407,
    STATUS_FW_UPGRADE_FW_IMAGE_CRC_INVALID                   = 408,
    STATUS_FW_UPGRADE_MCU_FLASH_WRITE_ERROR                  = 409,
    STATUS_FW_UPGRADE_MCU_FLASH_ERASE_ERROR                  = 410,
    STATUS_FW_UPGRADE_BOOT_FLAGS_NOT_PROGRAMMED_SUCCESSFULLY = 411,

    // errors for protocol
    STATUS_PACKET_WRONG_VERSION = 500,    ///< The wrong protocol version.
    STATUS_PACKET_WRONG_CRC     = 501,    ///< Received message has wrong CRC.
                                          ///< Duplicate STATUS_CRC_MSG_FAULT
    STATUS_PACKET_WRONG_SIZE = 502,       ///< Received message has wrong size.
                                          ///< Duplicate STATUS_TOO_BIG_SIZE
    STATUS_PACKET_WRONG_CHUNK_SIZE =
        503,    ///< Received chunked message with too big number of chunks.
    STATUS_PACKET_WRONG_CHUNK_INFO =
        504,    ///< Received chunked message with other client or communication
                ///< ID.
    STATUS_PACKET_WRONG_CHUNK_AMOUNT =
        505,    ///< Received chunked message with other chunk amount.
    STATUS_PACKET_WRONG_CHUNK_NUMBER =
        506,    ///< Received chunked message with other chunk number.
    STATUS_PACKET_CHUNK_FINISH     = 507,    ///< Received full chunked message.
    STATUS_PACKET_CHUNK_INPROGRESS = 508,    ///< Received chunk.
    STATUS_PACKET_STREAM_BUFFER_ERROR = 509,    ///< The stream buffer is full.
    STATUS_PACKET_SYNCHRO_LOST =
        510,    ///< The synchronization of frames was lost.
    STATUS_PACKET_BAD_MSG_ID      = 511,    ///< The message id is bad/unknown.
    STATUS_PACKET_UNKNOWN_COMMAND = 512,    ///< Unknown command.


    STATUS_PACKET_WRONG_MSG_CODE_RECEIVED =
        513, ///< The received message code does npt correspond to the sent message code.
    STATUS_PACKET_RECEPTION_TIMEOUT =
        514, ///< The timeout occured while waiting for the message reply.
    STATUS_PACKET_FAILED_WRITE_PORT = 515, ///< Failed to write to port.
    STATUS_PACKET_MSG_SEND_RETRY_EXCEEDED =
        516, ///< The number of send retries is equal to the maximum number of send retries.
    STATUS_PACKET_DROP_OF_PUSHED_MSG =
        517, ///< The sequential number received of push message is not the same as expected one.
    STATUS_PACKET_DROP_OF_RECEIVED_MSG =
        518, ///< The sequential number received is not the same as expected one.
    STATUS_PACKET_PUSH_FRAME_RECIEVED = 519,  ///< Received push packet
    STATUS_PACKET_EMPTY_REPLY_RECEIVED =
        520 ///< The empty message received(garbage).

} SystemStatus;
