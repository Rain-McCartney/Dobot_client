#include <QTimer>
#include "Protocol/Protocol.hpp"
#include "CommunicationService.hpp"
#include "CommunicationException.hpp"
#include "LoggingCategories/LoggingCategories.hpp"
#include "Communication/Model/SerialConfigStore.hpp"
#include "Communication/Model/SerialPortService.hpp"
#include "Communication/Data/CommunicationServiceData.hpp"

CommunicationService::CommunicationService(CommunicationServiceData *comData,
                                           Protocol* protocol,
                                           QObject *parent)
    : QObject{parent},
      m_comData(comData),
      m_protocol(protocol),
      m_sequenceNumber(0),
      m_isConnectedDevice(false),
      m_countDisconnected(0)
{
    SerialConfigStore* serialConfigStore = new SerialConfigStore(this);
    m_serialPortService = new SerialPortService(serialConfigStore, this);

    m_timerPing = new QTimer(this);
    m_watcherTimer = new QFutureWatcher<bool>(this);

    connect(m_serialPortService, &AbstractDevice::opened, this,  &CommunicationService::openedPort);
    connect(m_serialPortService, &AbstractDevice::closed,  this, &CommunicationService::closedPort);

    connect(m_timerPing,    &QTimer::timeout,                this, &CommunicationService::pingTimerTick);
    connect(m_watcherTimer, &QFutureWatcher<bool>::finished, this, &CommunicationService::finishedWatcherTimer);

    m_protocol->setPort(m_serialPortService);

}

SerialPortService *CommunicationService::getSerialPortService() const
{
    return m_serialPortService;
}

void CommunicationService::openDevice()
{
    m_serialPortService->open();
}

void CommunicationService::closeDevice()
{
    m_protocol->stopReading();
    m_serialPortService->close();
}

void CommunicationService::openedPort()
{
    qInfo(logInfo()) << "Port is opened";
    m_protocol->initializeReadFunction();
    this->startPingPolling();
    m_infoDevice.namePort = m_serialPortService->getName();
    emit openedDevice();
}

void CommunicationService::closedPort()
{
    qInfo(logInfo()) << "Port is closed";
    this->stopPingPolling();
    m_isConnectedDevice = false;
    m_infoDevice.statusEnabled = m_isConnectedDevice;
    emit isConnectedDeviceSignal(m_infoDevice);
    emit closedDevice();
}

void CommunicationService::pingTimerTick()
{
    if (m_watcherTimer->isRunning() == false)
    {
        QFuture<bool> future = m_comData->sendCommandPing(m_sequenceNumber);
        m_watcherTimer->setFuture(future);
        m_sequenceNumber++;
    }
    else
    {
        return;
    }
}

void CommunicationService::finishedWatcherTimer()
{
    try
    {
        if (m_watcherTimer->result())
        {
            if (m_isConnectedDevice)
            {
                qInfo(logInfo()) << "Connection established";
            }
            else
            {
                m_isConnectedDevice = true;
                m_infoDevice.statusEnabled = m_isConnectedDevice;
                emit isConnectedDeviceSignal(m_infoDevice);
                m_countDisconnected = 0;
                qInfo(logInfo()) << "Device connected";
            }
        }
        else
        {
            qWarning(logWarning()) << "CommunicationService::sengPing: payload responce from MCU failed";
        }
    }
    catch (CommunicationException& ex)
    {
        if (m_isConnectedDevice)
        {
            const uint8_t numberPingAttempt = 3;
            if (m_countDisconnected == numberPingAttempt)
            {
                m_isConnectedDevice = false;
                m_infoDevice.statusEnabled = m_isConnectedDevice;
                emit isConnectedDeviceSignal(m_infoDevice);
                qCritical(logCritical()) << "Device disconnected";
            }
            m_countDisconnected++;
        }
        else
        {
            qWarning(logWarning()) << "Unable to connect";
            ex.getMessage();
        }
    }
}

void CommunicationService::startPingPolling()
{
    m_timerPing->start(1000);
}

void CommunicationService::stopPingPolling()
{
    m_timerPing->stop();
}
