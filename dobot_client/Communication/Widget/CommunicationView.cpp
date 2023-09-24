#include <QPushButton>
#include "CommunicationView.hpp"
#include "Communication/Widget/SerialView.hpp"

CommunicationView::CommunicationView(QWidget *parent)
    : QWidget{parent}
{
    QVBoxLayout* pTopLayout = new QVBoxLayout(this);

    m_serialPortView = new SerialView(this);
    pTopLayout->addWidget(m_serialPortView);

    pTopLayout->addLayout(createButtonsLayout());
}

QHBoxLayout *CommunicationView::createButtonsLayout()
{
    QHBoxLayout* pTopLayout = new QHBoxLayout();

    m_openBtn = new QPushButton("Open");
    pTopLayout->addWidget(m_openBtn);

    m_closeBtn = new QPushButton("Close");
    pTopLayout->addWidget(m_closeBtn);


    return pTopLayout;
}
