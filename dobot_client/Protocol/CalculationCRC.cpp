#include <array>
#include "Protocol/CalculationCRC.h"

//*****************************************************************************
//! \brief CRC32 polynomial
//*****************************************************************************
const uint32_t CRC32Poly = 0x04C11DB7;

const uint32_t SizeOfCRC32Table = 256;

//*****************************************************************************
//! \brief Reflects data bits. A helper function for the CRC32 algorithm.
//!
//! \param data[in] is the data, bits of which should be reflected.
//! \param dataSize[in] size of the input data in bits.
//! \return uint32_t is the reflected data.
//!
//*****************************************************************************
uint32_t CRC32Reflect(uint32_t data, uint8_t dataSize)
{
    uint_fast32_t reflectedData = 0; // Clear accumulator variable.
    // Swap bit 0 for bit 7, bit 1 for bit 6, etc.
    for (int_fast16_t i = 1; i < (dataSize + 1); i++)
    {
        if (data & 1)
        {
            reflectedData |= 1 << (dataSize - i);
        }
        data >>= 1;
    }
    return reflectedData;
}

//--------------------------------------------------------------------------------

void InitCRC32(std::array<uint32_t, SizeOfCRC32Table>& CRC32Table)
{
    for (uint32_t i = 0; i < 256; i++)
    {
        CRC32Table[i] = CRC32Reflect(i, 8) << 24;
        for (uint32_t bit = 0; bit < 8; bit++)
        {
            if (CRC32Table[i] & (static_cast<uint32_t>(1) << 31))
            {
                CRC32Table[i] = (CRC32Table[i] << 1) ^ CRC32Poly;
            }
            else
            {
                CRC32Table[i] = CRC32Table[i] << 1;
            }
        }
        CRC32Table[i] = CRC32Reflect(CRC32Table[i], 32);
    }
}

//--------------------------------------------------------------------------------

uint32_t CalculateCRC32(const uint8_t* const data, uint32_t length, uint32_t crc)
{
    std::array<uint32_t, SizeOfCRC32Table> CRC32Table = std::array<uint32_t, SizeOfCRC32Table>();
    InitCRC32(CRC32Table);

    for (uint32_t i = 0; i < length; i++)
    {
        const uint8_t byte = data[i];

        crc = CRC32Table[(crc ^ byte) & 0xFF] ^ (crc >> 8);
    }

    return crc;
}

//--------------------------------------------------------------------------------

uint16_t CalculateCRC16(const uint8_t* data, uint32_t dataLength)
{
    // Initialize start vector.
    uint16_t crc = 0x1D0F;
    while (dataLength--)
    {
        // Produce CRC algorithm with 0x1021 polynomial.
        uint8_t temp = crc >> 8 ^ *data++;
        temp ^= temp >> 4;
        crc = (crc << 8) ^ static_cast<uint16_t>(temp << 12) ^ static_cast<uint16_t>(temp << 5) ^ static_cast<uint16_t>(temp);
    }
    return crc;
}
