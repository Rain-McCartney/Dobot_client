#include "Message.hpp"
#include "Protocol/ProtocolHelper.h"
#include "Protocol/CalculationCRC.h"

Message::Message(uint8_t packetLenght,
                 uint16_t sequenceNumber,
                 uint8_t comID,
                 uint16_t msgCode,
                 std::vector<uint8_t> payload,
                 QObject *parent)
    : QObject{parent},
      m_packetLength(packetLenght),
      m_comID(comID),
      m_sequenceNumber(sequenceNumber),
      m_messageCode(msgCode),
      m_fcs(0),
      m_payload(std::move(payload))

{
    this->createCRCAndData();
}

uint8_t Message::getPacketLenght() const
{
    return m_packetLength;
}

uint8_t Message::getComId() const
{
    return m_comID;
}

uint16_t Message::getSequenceNumber() const
{
    return m_sequenceNumber;
}

uint16_t Message::getMessageCode() const
{
    return m_messageCode;
}

uint32_t Message::getCrc32() const
{
    return m_fcs;
}

uint8_t *Message::getData()
{
    return m_data.data();
}

uint8_t Message::getDataSize() const
{
    return m_data.size();
}

uint8_t *Message::getPayloadData()
{
    return m_payload.data();
}

uint8_t Message::getPayloadSize() const
{
    return m_payload.size();
}

Message::Message(const uint8_t *message,
                 size_t receivedMessageSize)
{
    uint8_t messageWithoutCOBS[MaximumLengthInBytes];

    // ensure buffer is filled with Zero byte
    memset(messageWithoutCOBS, 0, MaximumLengthInBytes);
    memcpy(messageWithoutCOBS, message, receivedMessageSize);

    // IMPORTANT: message is already COBS decoded before coming here so no need to re-decode.
//    m_protocolVersion = messageFrameWithoutCOBS[ProtocolVersionOffsetInBytes + COBSFirstByteOffset];
    m_packetLength = messageWithoutCOBS[PacketLengthOffsetInBytes + COBSFirstByteOffset];
//    m_clientID = messageFrameWithoutCOBS[ClientIDOffsetInBytes + COBSFirstByteOffset];
    uint16_t seq_num_MSB = messageWithoutCOBS[SequenceNumberOffsetInBytes + COBSFirstByteOffset + 1] << 8u;
    uint16_t seq_num_LSB = messageWithoutCOBS[SequenceNumberOffsetInBytes + COBSFirstByteOffset];
    m_sequenceNumber = seq_num_MSB | seq_num_LSB;
    m_comID = messageWithoutCOBS[CommunicationIDOffsetInBytes + COBSFirstByteOffset];
//    m_flags = messageWithoutCOBS[FlagsOffsetInBytes + COBSFirstByteOffset];
//    m_reserved = messageFrameWithoutCOBS[ReservedOffsetInBytes + COBSFirstByteOffset];
    uint16_t msg_code_MSB = messageWithoutCOBS[MessageCodeOffsetInBytes + COBSFirstByteOffset + 1] << 8u;
    uint16_t msg_code_LSB = messageWithoutCOBS[MessageCodeOffsetInBytes + COBSFirstByteOffset];

    m_messageCode = msg_code_MSB | msg_code_LSB;

    const int bytes_stuffing_overhead = 2; // Overhead byte + EOP byte = 2 bytes
    const int payloadSize = static_cast<int>(receivedMessageSize) - MinimumLengthInByte - bytes_stuffing_overhead;
    if (payloadSize < 0)
    {
        throw std::length_error("Received frame too short");
    }
    const uint8_t* payload_to_copy_base_ptr = messageWithoutCOBS + COBSFirstByteOffset + PayloadOffsetInBytes;

    // the "last pointer" is excluded from copy by assign
    const uint8_t* payload_to_copy_last_ptr = payload_to_copy_base_ptr + payloadSize;

    m_payload.assign(payload_to_copy_base_ptr, payload_to_copy_last_ptr);
    m_data.assign(messageWithoutCOBS, messageWithoutCOBS + receivedMessageSize);

    this->createCRCAndData();
}

void Message::createCRCAndData()
{
    const uint32_t dataLengthBeforeCRC = PayloadOffsetInBytes + static_cast<uint32_t>(m_payload.size());
    // resize for frame size
    m_data.resize(dataLengthBeforeCRC);
    this->getMessageDataWithoutCRC(m_data.data());

    m_fcs = CalculateCRC32(m_data.data(), dataLengthBeforeCRC, 0xFFFFFFFF);
    // resize for CRC space
    m_data.resize(m_data.size() + 4);
    m_data[dataLengthBeforeCRC + 3] = m_fcs >> 24u;
    m_data[dataLengthBeforeCRC + 2] = (m_fcs >> 16u) & 0xFFu;
    m_data[dataLengthBeforeCRC + 1] = (m_fcs >> 8u) & 0xFFu;
    m_data[dataLengthBeforeCRC] = m_fcs & 0xFFu;

    m_data.resize(m_data.size() + 2);
    applyCOBS(m_data.data(), m_data.size());
}

void Message::getMessageDataWithoutCRC(uint8_t* frameWithoutCRC)
{
    frameWithoutCRC[PacketLengthOffsetInBytes] = m_packetLength;
    frameWithoutCRC[SequenceNumberOffsetInBytes] = static_cast<uint8_t>(m_sequenceNumber) & 0xFFu;
    frameWithoutCRC[SequenceNumberOffsetInBytes + 1] = m_sequenceNumber >> 8u;
    frameWithoutCRC[CommunicationIDOffsetInBytes] = m_comID;
    frameWithoutCRC[MessageCodeOffsetInBytes] = static_cast<uint8_t>(m_messageCode) & 0xFFu;
    frameWithoutCRC[MessageCodeOffsetInBytes + 1] = m_messageCode >> 8u;
    memcpy(frameWithoutCRC + PayloadOffsetInBytes, m_payload.data(), m_payload.size());
}

int32_t Message::intValueFromPayload() const
{
    int32_t payload;
    memcpy(&payload, m_payload.data(), sizeof(int32_t));
    return payload;
}
