#pragma once

#include <QSerialPort>

struct SerialPortSettings {
    QString                  name;
    QSerialPort::BaudRate    baudRate;
    QSerialPort::DataBits    dataBits;
    QSerialPort::Parity      parity;
    QSerialPort::StopBits    stopBits;
    QSerialPort::FlowControl flowControl;
};

Q_DECLARE_METATYPE(SerialPortSettings)
