
#pragma once

#include <QMainWindow>

class CommunicationView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:

    QWidget* m_centralWidget = nullptr;

    CommunicationView* m_communicationView = nullptr;
};
