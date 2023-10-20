#include <QMutex>
#include <QDebug>
#include <QIODevice>

#include "Errors.hpp"
#include "AbstractDevice.hpp"
#include "LoggingCategories/LoggingCategories.hpp"

AbstractDevice::AbstractDevice(QObject *parent)
    : QObject{parent}, AbstractPort()
{
    connect(this, &AbstractDevice::_writeSignal, this, &AbstractDevice::writeSlot, Qt::QueuedConnection);
}

int AbstractDevice::writeData(const uint8_t* data, int size)
{
    QByteArray bufer = QByteArray(reinterpret_cast<const char*>(data), size);
    emit _writeSignal(bufer);
    return static_cast<int>(STATUS_OK);
}

int AbstractDevice::readData(uint8_t *receiveMessage, int maxNumberOfBytesToReceive, int *numberOfReceivedBytes)
{
    m_readBufferMutex.lock();

    *numberOfReceivedBytes = (maxNumberOfBytesToReceive > m_readBuffer.size())
            ? m_readBuffer.size()
            : maxNumberOfBytesToReceive;

    if (*numberOfReceivedBytes > 0)
    {
        memcpy(receiveMessage,  m_readBuffer.data(),  *numberOfReceivedBytes);
        qDebug(logDebug()) << "Read: " << QByteArray(m_readBuffer, *numberOfReceivedBytes).toHex(' ');
        m_readBuffer.remove(0, *numberOfReceivedBytes);
    }

    m_readBufferMutex.unlock();

    return static_cast<int>(STATUS_OK);
}

void AbstractDevice::writeSlot(QByteArray data)
{
    if(m_device->isOpen())
    {
        m_device->write(data, data.size());
        qDebug(logDebug()) << "Send: " << data.toHex(' ');
    }
}

void AbstractDevice::onProtReadyRead()
{
    QByteArray data = m_device->readAll();

    m_readBufferMutex.lock();

    if (m_readBuffer.size() > s_readBufferSize && data.size() > s_readBufferSize)
    {
        qCritical(logCritical()) << "Too many bytes in the TCP socket buffer.";
        m_readBuffer = QByteArray(data);

    }
    else if (m_readBuffer.size() > s_readBufferSize) {
        m_readBuffer.remove(0, data.size());
        m_readBuffer.append(data);

    }
    else if (data.size() > 0) {
        m_readBuffer.append(data);
    }

    m_readBufferMutex.unlock();
}
