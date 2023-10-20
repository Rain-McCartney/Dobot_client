
#pragma once

#include <QVBoxLayout>
#include <QMainWindow>

class Protocol;
class LogWindow;
class CommandHandler;
class StatusBarWidget;
class CommunicationView;
class CommunicationService;
class CommunicationServiceData;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static void logHandler(QtMsgType type, const QMessageLogContext& context, const QString &msg);


private:

    QWidget* m_centralWidget = nullptr;

    Protocol* m_protocol = nullptr;
    CommandHandler* m_comHandler = nullptr;

    CommunicationServiceData* m_communicationServiceData = nullptr;
    CommunicationService* m_communicationService = nullptr;
    CommunicationView*    m_communicationView    = nullptr;
    StatusBarWidget* m_statusBarWidget = nullptr;

    static LogWindow* m_logWindow;

    QVBoxLayout *createWidgetLayout();
};
