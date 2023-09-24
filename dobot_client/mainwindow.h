
#pragma once

#include <QMainWindow>

class Protocol;
class CommandHandler;
class CommunicationView;
class CommunicationService;
class CommunicationServiceData;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:

    QWidget* m_centralWidget = nullptr;

    Protocol* m_protocol = nullptr;
    CommandHandler* m_comHandler = nullptr;

    CommunicationServiceData* m_communicationServiceData = nullptr;
    CommunicationService* m_communicationService = nullptr;
    CommunicationView*    m_communicationView    = nullptr;
};
