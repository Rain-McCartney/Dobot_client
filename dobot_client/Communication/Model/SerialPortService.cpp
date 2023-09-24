#include "SerialPortService.hpp"

SerialPortService::SerialPortService(SerialConfigStore* configStore,
                                     QObject *parent)
    : AbstractDevice{parent}
{
    m_device = new QSerialPort(this);

    connect(m_device, &QSerialPort::readyRead, this, &AbstractDevice::onProtReadyRead);
    m_settingsPort = configStore->getSettings();
}

void SerialPortService::open()
{
    QSerialPort* port = static_cast<QSerialPort*>(m_device);
    port->setPortName(m_settingsPort.name);
    if (port->open(QIODevice::ReadWrite)) {
        if (port->setBaudRate(m_settingsPort.baudRate)
         && port->setDataBits(m_settingsPort.dataBits)
         && port->setParity(m_settingsPort.parity)
         && port->setStopBits(m_settingsPort.stopBits)
         && port->setFlowControl(m_settingsPort.flowControl)) {
            if (port->isOpen()){
               emit opened();
            }
        }
        else {
            port->close();
            emit closed();
        }
    }
    else {
       port->close();
       emit closed();
    }
}

void SerialPortService::close()
{
    if (m_device->isOpen())
    {
        m_device->close();
        emit closed();
    }
}

QString SerialPortService::getName() const
{
    return static_cast<QSerialPort*>(m_device)->portName();
}

void SerialPortService::setName(const QString &name)
{
    m_settingsPort.name = name;
    emit settingsChanged(m_settingsPort);
}

void SerialPortService::emitState()
{
    emit settingsChanged(m_settingsPort);
}
