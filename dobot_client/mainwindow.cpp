#include <QGridLayout>
#include "mainwindow.h"
#include "Communication/Widget/CommunicationView.hpp"
#include "Communication/Model/CommunicationService.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_centralWidget = new QWidget(this);
    QGridLayout* pCentralLayout = new QGridLayout(m_centralWidget);

    m_communicationService = new CommunicationService(this);

    m_communicationView = new CommunicationView(m_communicationService, this);
    pCentralLayout->addWidget(m_communicationView);

    this->setCentralWidget(m_centralWidget);

}

MainWindow::~MainWindow()
{
}

