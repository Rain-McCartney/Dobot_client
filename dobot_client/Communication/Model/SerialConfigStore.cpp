#include <QSettings>
#include "SerialConfigStore.hpp"
#include "Communication/Model/SerialPortSettigs.h"

SerialConfigStore::SerialConfigStore(QObject *parent)
    : QObject{parent}
{
    m_appSettings = new QSettings(this);
}

SerialPortSettings SerialConfigStore::getSettings()
{
    SerialPortSettings settings;
    settings.name = m_appSettings->value("SerialPort/Name", "").toString();
    settings.baudRate = static_cast<QSerialPort::BaudRate>(m_appSettings->value("SerialPort/BaudRate", "115200").toInt());
    settings.dataBits = static_cast<QSerialPort::DataBits>(m_appSettings->value("SerialPort/DataBits", "8").toInt());
    settings.parity = static_cast<QSerialPort::Parity>(m_appSettings->value("SerialPort/Parity", "0").toInt()); // It is QT enum Parity
    settings.stopBits = static_cast<QSerialPort::StopBits>(m_appSettings->value("SerialPort/StopBits", "1").toInt());
    settings.flowControl = static_cast<QSerialPort::FlowControl>(m_appSettings->value("SerialPort/FlowControl", "0").toInt());

    return settings;
}
