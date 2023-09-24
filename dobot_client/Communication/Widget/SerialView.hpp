
#pragma once

#include <QWidget>

class QLabel;
class QComboBox;
class QGroupBox;

class SerialView : public QWidget
{
    Q_OBJECT
public:
    explicit SerialView(QWidget *parent = nullptr);

signals:


private:

    QGroupBox* m_infoPort = nullptr;

    QComboBox* m_selectPortComboBox = nullptr;

    QLabel* m_serialNumberLabel;
    QLabel* m_descriptionLabel;
    QLabel* m_manufacturerLabel;
    QLabel* m_locationLabel;
    QLabel* m_pidLabel;
    QLabel* m_vidLabel;

    QGroupBox* createSelectPortBox();

};
