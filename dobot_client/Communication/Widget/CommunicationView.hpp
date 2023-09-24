
#pragma once

#include <QWidget>
#include <QHBoxLayout>

class SerialView;
class QPushButton;

class CommunicationView : public QWidget
{
    Q_OBJECT
public:
    explicit CommunicationView(QWidget *parent = nullptr);

signals:

private:

    SerialView* m_serialPortView = nullptr;

    QPushButton* m_openBtn  = nullptr;
    QPushButton* m_closeBtn = nullptr;

    QHBoxLayout* createButtonsLayout();


};

