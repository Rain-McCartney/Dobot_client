
#pragma once

#pragma once

#include <QObject>
#include <QSerialPort>

class QSettings;
class SerialPortSettings;

class SerialConfigStore : public QObject
{
    Q_OBJECT
public:
    explicit SerialConfigStore(QObject *parent = nullptr);

    SerialPortSettings getSettings();

private:

    QSettings* m_appSettings;
};

