
#pragma once

#include <QWidget>

#include "Communication/Model/CommunicationService.hpp"

class QLabel;
class LedWidget;
class QVBoxLayout;
class StatusBarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StatusBarWidget(CommunicationService* comService, QWidget *parent = nullptr);


public slots:

//    void deviceDicconnected(const InfoDevice& infoDevice);

//    void deviceConnected(const InfoDevice& infoDevice);

signals:

private slots:

    void isUpdateStatusBar(const InfoDevice& infoDevice);

private:

    CommunicationService* m_comService = nullptr;

    QLabel* m_typePortLabel = nullptr;

    QLabel* m_namePortLabel = nullptr;

    LedWidget* m_ledWgt = nullptr;

    QString m_typePort;

    QVBoxLayout *createParameterLayout();

};
