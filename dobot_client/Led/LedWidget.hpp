
#pragma once

#include <QWidget>
#include <QPixmap>
#include <QLabel>
#include <QLayout>

class LedWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LedWidget(const QString& name, QWidget *parent = nullptr);

signals:

private:

    QLabel *m_labelLed;
    QLabel *m_name;

    static inline const QString s_greenColor = ":/Images/Green.png";
    static inline const QString s_blackColor = ":/Images/Black.png";

public slots:

    void setState(bool value);

};

