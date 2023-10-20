#include <QLabel>
#include <QVBoxLayout>
#include "Led/LedWidget.hpp"
#include "StatusBarWidget.hpp"

StatusBarWidget::StatusBarWidget(CommunicationService *comService, QWidget *parent)
    : QWidget{parent}, m_comService(comService)
{
    m_typePort = "Type Port:";

    QHBoxLayout* pTopLayout = new QHBoxLayout(this);

    pTopLayout->addLayout(createParameterLayout());
    pTopLayout->setContentsMargins(0, 0, 0, 0);

    connect(m_comService, &CommunicationService::isConnectedDeviceSignal, this, &StatusBarWidget::isUpdateStatusBar);

//    connect(m_comService, &CommunicationService::connectedDevice,    this, &StatusBarWidget::deviceConnected);
    //    connect(m_comService, &CommunicationService::disconnectedDevice, this, &StatusBarWidget::deviceDicconnected);
}

void StatusBarWidget::isUpdateStatusBar(const InfoDevice &infoDevice)
{
    if (infoDevice.statusEnabled == true)
    {
        m_namePortLabel->setText(infoDevice.namePort);
        m_typePortLabel->setText(infoDevice.typePort);
        m_ledWgt->setState(infoDevice.statusEnabled);
    }
    else
    {
        m_namePortLabel->setText("-");
        m_typePortLabel->setText(m_typePort);
        m_ledWgt->setState(infoDevice.statusEnabled);
    }
}

//void StatusBarWidget::deviceDicconnected(const InfoDevice &infoDevice)
//{
//    m_namePortLabel->setText("-");
//    m_typePortLabel->setText(m_typePort);
//    m_ledWgt->setState(infoDevice.statusEnabled);
//}

//void StatusBarWidget::deviceConnected(const InfoDevice &infoDevice)
//{
//    m_namePortLabel->setText(infoDevice.namePort);
//    m_typePortLabel->setText(infoDevice.typePort);
//    m_ledWgt->setState(infoDevice.statusEnabled);
//}

QVBoxLayout *StatusBarWidget::createParameterLayout()
{
    QVBoxLayout* pTopLayout = new QVBoxLayout();

    m_ledWgt = new LedWidget("Connection", this);
    pTopLayout->addWidget(m_ledWgt, 3, Qt::AlignLeft);

    QHBoxLayout* pLabelLayout = new QHBoxLayout();

    m_typePortLabel = new QLabel(m_typePort, this);
    pLabelLayout->addWidget(m_typePortLabel);

    m_namePortLabel = new QLabel("-", this);
    pLabelLayout->addWidget(m_namePortLabel, 3, Qt::AlignLeft);
    pTopLayout->addLayout(pLabelLayout);
    pTopLayout->setContentsMargins(0, 0, 0, 0);

    return pTopLayout;
}
