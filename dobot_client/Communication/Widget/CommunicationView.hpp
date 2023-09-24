
#pragma once

#include <QWidget>
#include <QHBoxLayout>

class SerialView;
class QPushButton;
class CommunicationService;

class CommunicationView : public QWidget
{
    Q_OBJECT
public:
    explicit CommunicationView(CommunicationService* comService,
                               QWidget *parent = nullptr);

signals:

private slots:

    void onPortIsOpened();
    void onPortIsClosed();

private:

    CommunicationService* m_communicationService = nullptr;

    SerialView* m_serialPortView = nullptr;

    QPushButton* m_openBtn  = nullptr;
    QPushButton* m_closeBtn = nullptr;

    QHBoxLayout* createButtonsLayout();

    void setOpenedState(bool isOpened);


};

