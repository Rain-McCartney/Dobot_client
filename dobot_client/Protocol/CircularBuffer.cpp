#include "CircularBuffer.hpp"

CircularBuffer::CircularBuffer(uint16_t size,
                               QObject *parent)
    : QObject{parent},
      m_buffer(size, 0xFF),
      m_readIndex(0),
      m_writeIndex(0),
      m_exclusiveReadWrite()
{

}

CircularBuffer::~CircularBuffer()
{
    m_buffer.clear();
}

size_t CircularBuffer::writeReceiveData(uint8_t *receiveMessageBuffer, uint32_t numberOfBytes)
{
    std::unique_lock<std::mutex> lock(m_exclusiveReadWrite);
    size_t written = 0;
    for (size_t i = 0; i < numberOfBytes; ++i)
    {
        uint8_t to_write = receiveMessageBuffer[i];
        this->innerWrite(receiveMessageBuffer[i]);
        ++written;
        if (to_write == ZeroPadding)
        {
            break;
        }
    }
    return written;
}

retStatusCode CircularBuffer::getReceivedMessage(uint8_t *receivedMessage,
                                                 uint32_t &receivedMessageSize,
                                                 uint32_t maxMessageSize)
{
    std::unique_lock<std::mutex> lock(m_exclusiveReadWrite);
    const size_t EOP_NB_BYTE = 1;
    receivedMessageSize = 0;
    if (innerFindZerroPadding())
    {
        uint32_t i = 0;
        for (; i < maxMessageSize; ++i)
        {
            receivedMessage[i] = innerRead();
            if (receivedMessage[i] == ZeroPadding)
            {
                receivedMessageSize = i + EOP_NB_BYTE;
                return STATUS_OK;
            }
        }
        // Buffer contain too many bytes.
        while (innerRead() != ZeroPadding)
        {
            ++i;
        }
        receivedMessageSize = i + EOP_NB_BYTE;
    }
    return STATUS_ERROR;
}

void CircularBuffer::innerWrite(uint8_t input)
{
    m_buffer[m_writeIndex++] = input;
    if (m_writeIndex == m_buffer.size())
    {
        m_writeIndex = 0;
    }
}

uint8_t CircularBuffer::innerRead()
{
    const uint8_t value = m_buffer[m_readIndex++];
    if (m_readIndex == m_buffer.size())
    {
        m_readIndex = 0;
    }
    return value;
}

bool CircularBuffer::innerFindZerroPadding()
{
    if (m_readIndex == m_writeIndex)
    {
        return false;
    }
    else
    {
        if (m_readIndex < m_writeIndex)
        {
            const std::vector<uint8_t>::iterator it = std::find(m_buffer.begin() + m_readIndex,
                                                                m_buffer.begin() + m_writeIndex,
                                                                ZeroPadding);

            return (it != m_buffer.begin() + m_writeIndex);
        }
        else
        {
            auto it = std::find(m_buffer.begin() + m_readIndex,
                                m_buffer.end(),
                                ZeroPadding);
            if (it != m_buffer.end())
            {
                return true;
            }
            else
            {
                it = std::find(m_buffer.begin(),
                               m_buffer.begin() + m_writeIndex,
                               ZeroPadding);
                return (it != m_buffer.begin() + m_writeIndex);
            }
        }
    }

}
