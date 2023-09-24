
#pragma once

#include <QObject>
#include <QFutureWatcher>

class QTimer;
class Protocol;
class SerialPortService;
class CommunicationServiceData;

class CommunicationService : public QObject
{
    Q_OBJECT
public:
    explicit CommunicationService(CommunicationServiceData* comData,
                                  Protocol* protocol,
                                  QObject *parent = nullptr);

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

    void pingTimerTick();
    void finishedWatcherTimer();

    void startPingPolling();
    void stopPingPolling();

private:

    CommunicationServiceData* m_comData = nullptr;
    Protocol* m_protocol = nullptr;

    SerialPortService* m_serialPortService = nullptr;

    QTimer* m_timerPing = nullptr;
    QFutureWatcher<uint32_t>* m_watcherTimer = nullptr;

    quint32 m_sequenceNumber;

};

