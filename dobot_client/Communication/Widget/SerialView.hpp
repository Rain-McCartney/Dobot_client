
#pragma once

#include <QWidget>
#include "Communication/Model/SerialPortSettigs.h"

class QLabel;
class QComboBox;
class QGroupBox;
class SerialPortService;

class SerialView : public QWidget
{
    Q_OBJECT
public:
    explicit SerialView(SerialPortService* serialPortService,
                        QWidget *parent = nullptr);

signals:

private slots:

    void settingsModelChanged(const SerialPortSettings& settings);

    void onNameChanged();

private:

    SerialPortService* m_serialPortService = nullptr;

    QGroupBox* m_infoPort = nullptr;

    QComboBox* m_selectPortComboBox = nullptr;

    QLabel* m_serialNumberLabel;
    QLabel* m_descriptionLabel;
    QLabel* m_manufacturerLabel;
    QLabel* m_locationLabel;
    QLabel* m_pidLabel;
    QLabel* m_vidLabel;

    QGroupBox* createSelectPortBox();

    void fillPortsInfo();
    void setPortName(const QString &name);

    void showPortInfo(int index);

};
