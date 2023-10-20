#include <QHBoxLayout>
#include "mainwindow.h"
#include "Protocol/Protocol.hpp"
#include "LogWindow/LogWindow.hpp"
#include "Protocol/CommandHandler.hpp"
#include "StatusBar/StatusBarWidget.hpp"
#include "Communication/Widget/CommunicationView.hpp"
#include "Communication/Model/CommunicationService.hpp"
#include "Communication/Data/CommunicationServiceData.hpp"

LogWindow *MainWindow::m_logWindow = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_centralWidget = new QWidget(this);
    QHBoxLayout* pCentralLayout = new QHBoxLayout(m_centralWidget);

    m_protocol = new Protocol(this);

    m_comHandler = new CommandHandler(m_protocol, this);

    m_communicationServiceData = new CommunicationServiceData(m_comHandler, this);

    m_communicationService = new CommunicationService(m_communicationServiceData,
                                                      m_protocol,
                                                      this);
    pCentralLayout->addLayout(createWidgetLayout());

    this->setCentralWidget(m_centralWidget);
}

MainWindow::~MainWindow()
{
}

void MainWindow::logHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    const QString& category = context.category;
    switch (type)
    {
    case QtInfoMsg:
        m_logWindow->printInfoMessage(category, msg);
        break;
    case QtDebugMsg:
        m_logWindow->printDebugMessage(category, msg);
        break;
    case QtWarningMsg:
        m_logWindow->printWarningMessage(category, msg);
        break;
    case QtCriticalMsg:
        m_logWindow->printErrorMessage(category, msg);
        break;
    case QtFatalMsg:
        m_logWindow->printErrorMessage(category, msg);
        break;
    }
}

QVBoxLayout* MainWindow::createWidgetLayout()
{
    QVBoxLayout* pTopLayout = new QVBoxLayout();
    m_statusBarWidget = new StatusBarWidget(m_communicationService, this);
    pTopLayout->addWidget(m_statusBarWidget);

    m_communicationView = new CommunicationView(m_communicationService, this);
    pTopLayout->addWidget(m_communicationView);

    m_logWindow = new LogWindow(m_centralWidget);
    pTopLayout->addWidget(m_logWindow, 2);
    pTopLayout->setContentsMargins(0, 0, 0, 0);

    return pTopLayout;
}
