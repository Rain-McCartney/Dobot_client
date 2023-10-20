
#include "LedWidget.hpp"

LedWidget::LedWidget(const QString &name, QWidget *parent)
    : QWidget{parent}
{
    QGridLayout* pTopLayout = new QGridLayout(this);
    m_labelLed = new QLabel(this);
    m_labelLed->setMinimumSize(25, 25);
    m_labelLed->setMaximumSize(25, 25);
    m_labelLed->setScaledContents(true);
    m_labelLed->setPixmap(QPixmap(":/Images/Black.png"));

    m_name = new QLabel(name);


    pTopLayout->addWidget(m_name, 0, 1, 1, 1);
    pTopLayout->addWidget(m_labelLed, 0, 2, 1, 1);

    pTopLayout->setContentsMargins(0, 0, 0, 0);

}

void LedWidget::setState(bool value)
{
    if (value)
    {
        m_labelLed->setPixmap(QPixmap(s_greenColor));
    }
    else
    {
        m_labelLed->setPixmap(QPixmap(s_blackColor));
    }

}

