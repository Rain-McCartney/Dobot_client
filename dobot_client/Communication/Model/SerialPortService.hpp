
#pragma once

#include <QObject>
#include <QSerialPort>

#include "Communication/Model/AbstractDevice.hpp"
#include "Communication/Model/SerialPortSettigs.h"
#include "Communication/Model/SerialConfigStore.hpp"

class SerialPortService : public AbstractDevice
{
    Q_OBJECT
public:
    explicit SerialPortService(SerialConfigStore *configStore,
                               QObject *parent = nullptr);

    void open()  override;
    void close() override;

    QString getName() const override;

    void setName(const QString& name);

public slots:

    void emitState();

signals:

    void settingsChanged(const SerialPortSettings& settings);

private:

    SerialPortSettings m_settingsPort;

};

