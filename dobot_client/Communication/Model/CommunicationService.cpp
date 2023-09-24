#include "CommunicationService.hpp"
#include "Communication/Model/SerialConfigStore.hpp"
#include "Communication/Model/SerialPortService.hpp"

CommunicationService::CommunicationService(QObject *parent)
    : QObject{parent}
{
    SerialConfigStore* serialConfigStore = new SerialConfigStore(this);
    m_serialPortService = new SerialPortService(serialConfigStore, this);

    connect(m_serialPortService, &AbstractDevice::opened, this,  &CommunicationService::openedPort);
    connect(m_serialPortService, &AbstractDevice::closed,  this, &CommunicationService::closedPort);

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
    m_serialPortService->close();
}

void CommunicationService::openedPort()
{
//    qInfo(logInfo()) << "Port is opened";
//    m_protocol->Initialize();
//    startPingPolling();
//    m_infoDevice.namePort = m_SerialService->getName();
    emit openedDevice();
}

void CommunicationService::closedPort()
{
//    qInfo(logInfo()) << "Port is closed";
//    stopPingPolling();
//    m_isConnected = false;
//    m_infoDevice.statusEnabled = false;
//    emit disconnectedDevice(m_infoDevice);
    emit closedDevice();
}
