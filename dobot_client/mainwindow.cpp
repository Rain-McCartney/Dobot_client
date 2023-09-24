#include <QGridLayout>
#include "mainwindow.h"
#include "Communication/Widget/CommunicationView.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_centralWidget = new QWidget(this);
    QGridLayout* pCentralLayout = new QGridLayout(m_centralWidget);

    m_communicationView = new CommunicationView(this);
    pCentralLayout->addWidget(m_communicationView);

    this->setCentralWidget(m_centralWidget);

}

MainWindow::~MainWindow()
{
}

