#include <QTimer>
#include "Protocol/Protocol.hpp"
#include "CommunicationService.hpp"
#include "Communication/Model/SerialConfigStore.hpp"
#include "Communication/Model/SerialPortService.hpp"
#include "Communication/Data/CommunicationServiceData.hpp"

CommunicationService::CommunicationService(CommunicationServiceData *comData,
                                           Protocol* protocol,
                                           QObject *parent)
    : QObject{parent},
      m_comData(comData),
      m_protocol(protocol),
      m_sequenceNumber(0)
{
    SerialConfigStore* serialConfigStore = new SerialConfigStore(this);
    m_serialPortService = new SerialPortService(serialConfigStore, this);

    m_timerPing = new QTimer(this);
    m_watcherTimer = new QFutureWatcher<uint32_t>(this);

    connect(m_serialPortService, &AbstractDevice::opened, this,  &CommunicationService::openedPort);
    connect(m_serialPortService, &AbstractDevice::closed,  this, &CommunicationService::closedPort);

    connect(m_timerPing,    &QTimer::timeout,                    this, &CommunicationService::pingTimerTick);
    connect(m_watcherTimer, &QFutureWatcher<uint32_t>::finished, this, &CommunicationService::finishedWatcherTimer);

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
//    qInfo(logInfo()) << "Port is opened";
    m_protocol->initializeReadFunction();
    this->startPingPolling();
//    m_infoDevice.namePort = m_SerialService->getName();
    emit openedDevice();
}

void CommunicationService::closedPort()
{
//    qInfo(logInfo()) << "Port is closed";
    this->stopPingPolling();
//    m_isConnected = false;
//    m_infoDevice.statusEnabled = false;
//    emit disconnectedDevice(m_infoDevice);
    emit closedDevice();
}

void CommunicationService::pingTimerTick()
{
    if (m_watcherTimer->isRunning() == false)
    {
        QFuture<uint32_t> future = m_comData->sendCommandPing(m_sequenceNumber);
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
    uint32_t type = m_watcherTimer->result();
}

void CommunicationService::startPingPolling()
{
    m_timerPing->start(2000);
}

void CommunicationService::stopPingPolling()
{
    m_timerPing->stop();
}
