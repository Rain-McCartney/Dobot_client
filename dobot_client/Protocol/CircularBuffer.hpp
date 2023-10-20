
#pragma once

#include <mutex>
#include <memory>
#include <QObject>
#include "Protocol/ProtocolHelper.h"

class CircularBuffer : public QObject
{
    Q_OBJECT
public:
    explicit CircularBuffer(uint16_t size,
                            QObject *parent = nullptr);

    ~CircularBuffer();

    size_t writeReceiveData(uint8_t* receiveMessageBuffer,
                             uint32_t numberOfBytes);
    retStatusCode getReceivedMessage(uint8_t* receivedMessage,
                                     uint32_t& receivedMessageSize,
                                     uint32_t maxMessageSize);

private:

    std::vector<uint8_t> m_buffer;
    uint8_t m_readIndex;
    uint8_t m_writeIndex;

    std::mutex m_exclusiveReadWrite;

    void innerWrite(uint8_t input);
    uint8_t innerRead();
    bool innerFindZerroPadding();

};

typedef std::unique_ptr<CircularBuffer> CircularBufferUniquePtr;
