
#pragma once

#include <QObject>
#include <memory>
#include "Protocol/MessagePayload.hpp"


class Message : public QObject
{
    Q_OBJECT
public:
    explicit Message(uint8_t packetLenght,
                     uint16_t sequenceNumber,
                     uint8_t comID,
                     uint16_t msgCode,
                     std::vector<uint8_t> payload,
                     QObject *parent = nullptr);

    uint8_t getPacketLenght() const;
    uint8_t getComId() const;
    uint16_t getSequenceNumber() const;
    uint16_t getMessageCode() const;
    uint32_t getCrc32() const;
    uint8_t* getData();
    uint8_t getDataSize() const;


private:

    uint8_t m_packetLength; /**< The length of message frame . */

    uint8_t m_comID; /**< Communication ID (message sequence number). */

    uint16_t m_sequenceNumber; /**< Frame sequence number. */

    uint16_t m_messageCode; /**< The field that contains a message(command) code. */

    uint32_t m_fcs; /**< The CRC32 sum. It will be placed at the end of packet. */

    std::vector<uint8_t> m_payload;
    std::vector<uint8_t> m_data;

    void createCRCAndData();

    void getMessageDataWithoutCRC(uint8_t *frameWithoutCRC);
};

typedef std::shared_ptr<Message> MessageSmartPtr;


