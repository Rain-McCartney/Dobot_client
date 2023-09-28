
#pragma once

#include <cstdint>

/*****************************************************************************
 * \brief Initial value for the CRC32 algorithm. Should be passed as the
 *  crc parameter to the \ref mcb_sw_crc32_calculate function.
 ****************************************************************************/
const uint32_t CRC32InitValue = 0xFFFFFFFF;

/*****************************************************************************
 * \brief CRC32 verification value for the string '0123456789' without the
 * null byte.
 ****************************************************************************/
const uint32_t CRC32CheckValue = 0x340BC6D9;

/*****************************************************************************
 * \brief Initializes the lookup table used in calculating the CRC32 value.
 *
 ****************************************************************************/
void InitCRC32();

/*****************************************************************************
 * \brief Calculates CRC32 value for the supplied block of data.
 * The implemented CRC32 algorithm is CRC-32/JAMCRC with the following parameters:
 * Width: 32 bits
 * Poly: 0x4C11DB7
 * Init: 0xFFFFFFFF
 * RefIn: true
 * RefOut: true
 * XorOut: 0x0
 * Check: 0x340BC6D9
 * Note: The Check value is for the string '123456789' without finishing null byte.
 * The digits 1-9 are ASCII symbols.
 *
 * \param data[in] is a pointer to the supplied block of data.
 * \param length[in] length is the supplied data block length in bytes.
 * \param crc[in] is the CRC initial value. For a new CRC calculation it should be
 * 0xFFFFFFFF, for a continued CRC calculation it should be the CRC value, returned by the previous
 * mcb_hw_crc32_calculate funcion call.
 * \return uint32_t is the CRC32 calculation result.
 *****************************************************************************/
uint32_t CalculateCRC32(const uint8_t* data, uint32_t length, uint32_t crc);

/*****************************************************************************
 * \brief Calculates CRC16 value for the supplied block of data.
 *
 * Width: 32 bits
 * Poly: 0x4C11DB7
 * \param[in] data - pointer to the supplied block of data
 * \param[in] dataLength - the supplied data block length in bytes.
 * \return uint16_t is the CRC16 calculation result.
 ******************************************************************************/
uint16_t CalculateCRC16(const uint8_t* data, uint32_t dataLength);
