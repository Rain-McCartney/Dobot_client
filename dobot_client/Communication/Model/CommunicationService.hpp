
#pragma once

#include <QObject>

class SerialPortService;

class CommunicationService : public QObject
{
    Q_OBJECT
public:
    explicit CommunicationService(QObject *parent = nullptr);

    SerialPortService* getSerialPortService() const;

public slots:

    void openDevice();
    void closeDevice();

signals:

    void openedDevice();
    void closedDevice();

private slots:

    void openedPort();
    void closedPort();


private:


    SerialPortService* m_serialPortService = nullptr;

};

