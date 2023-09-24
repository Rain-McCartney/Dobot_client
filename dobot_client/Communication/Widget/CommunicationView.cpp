#include <QPushButton>
#include "CommunicationView.hpp"
#include "Communication/Widget/SerialView.hpp"
#include "Communication/Model/CommunicationService.hpp"

CommunicationView::CommunicationView(CommunicationService *comService,
                                     QWidget *parent)
    : QWidget{parent},
       m_communicationService(comService)
{
    QVBoxLayout* pTopLayout = new QVBoxLayout(this);

    m_serialPortView = new SerialView(m_communicationService->getSerialPortService(), this);
    pTopLayout->addWidget(m_serialPortView);

    pTopLayout->addLayout(createButtonsLayout());

    connect(m_openBtn,  &QPushButton::clicked, m_communicationService, &CommunicationService::openDevice);
    connect(m_closeBtn, &QPushButton::clicked, m_communicationService, &CommunicationService::closeDevice);

    connect(m_communicationService, &CommunicationService::openedDevice, this, &CommunicationView::onPortIsOpened);
    connect(m_communicationService, &CommunicationService::closedDevice, this, &CommunicationView::onPortIsClosed);

}

void CommunicationView::onPortIsOpened()
{
    this->setOpenedState(true);
}

void CommunicationView::onPortIsClosed()
{
    this->setOpenedState(false);
}

QHBoxLayout *CommunicationView::createButtonsLayout()
{
    QHBoxLayout* pTopLayout = new QHBoxLayout();

    m_openBtn = new QPushButton("Open");
    pTopLayout->addWidget(m_openBtn);

    m_closeBtn = new QPushButton("Close");
    m_closeBtn->setEnabled(false);
    pTopLayout->addWidget(m_closeBtn);


    return pTopLayout;
}

void CommunicationView::setOpenedState(bool isOpened)
{
    m_closeBtn->setEnabled(isOpened);
    m_openBtn->setEnabled(!isOpened);
//    m_openBtn->setEnabled(!isOpened);
//    m_serialRadioBtn->setEnabled(!isOpened);
//    m_tcpRadioBtn->setEnabled(!isOpened);
//    m_aardvarkRadioBtn->setEnabled(!isOpened);
//    m_serialWgt->setEnabled(!isOpened);
//    m_tcpWgt->setEnabled(!isOpened);
//    m_timeoutSpinBox->setEnabled(!isOpened);
}
