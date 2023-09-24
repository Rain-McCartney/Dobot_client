#include <QGridLayout>
#include "mainwindow.h"
#include "Protocol/Protocol.hpp"
#include "Protocol/CommandHandler.hpp"
#include "Communication/Widget/CommunicationView.hpp"
#include "Communication/Model/CommunicationService.hpp"
#include "Communication/Data/CommunicationServiceData.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_centralWidget = new QWidget(this);
    QGridLayout* pCentralLayout = new QGridLayout(m_centralWidget);

    m_protocol = new Protocol(this);

    m_comHandler = new CommandHandler(m_protocol, this);

    m_communicationServiceData = new CommunicationServiceData(m_comHandler, this);

    m_communicationService = new CommunicationService(m_communicationServiceData,
                                                      m_protocol,
                                                      this);

    m_communicationView = new CommunicationView(m_communicationService, this);
    pCentralLayout->addWidget(m_communicationView);

    this->setCentralWidget(m_centralWidget);

}

MainWindow::~MainWindow()
{
}

