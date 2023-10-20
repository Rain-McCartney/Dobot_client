
#pragma once

#include <QObject>
#include <QFutureWatcher>

class QTimer;
class Protocol;
class SerialPortService;
class CommunicationServiceData;

struct InfoDevice
{
    QString typePort;
    QString namePort;
    bool statusEnabled;
};

Q_DECLARE_METATYPE(InfoDevice)

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

    void isConnectedDeviceSignal(const InfoDevice& infoDevice);

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
    quint32 m_sequenceNumber;

    bool m_isConnectedDevice;
    uint8_t m_countDisconnected;

    QTimer* m_timerPing = nullptr;
    QFutureWatcher<bool>* m_watcherTimer = nullptr;

    InfoDevice m_infoDevice;

};
