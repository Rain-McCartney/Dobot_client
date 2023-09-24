#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QSerialPortInfo>
#include "SerialView.hpp"

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SerialView::SerialView(QWidget *parent)
    : QWidget{parent}
{
    QGridLayout* pTopLayout = new QGridLayout(this);

    m_infoPort = createSelectPortBox();
    pTopLayout->addWidget(m_infoPort);
    pTopLayout->setContentsMargins(0, 0, 0, 0);
}

QGroupBox *SerialView::createSelectPortBox()
{
    QGroupBox* box = new QGroupBox(this);
    QVBoxLayout* pTopLayout = new QVBoxLayout(box);

    m_selectPortComboBox = new QComboBox(box);
    pTopLayout->addWidget(m_selectPortComboBox);

    m_serialNumberLabel = new QLabel("Serial Number:",      box);
    pTopLayout->addWidget(m_serialNumberLabel);

    m_descriptionLabel  = new QLabel("Description:",        box);
    pTopLayout->addWidget(m_descriptionLabel);

    m_manufacturerLabel = new QLabel("Manufacturer:",       box);
    pTopLayout->addWidget(m_manufacturerLabel);

    m_locationLabel     = new QLabel("Location:",           box);
    pTopLayout->addWidget(m_locationLabel);

    m_pidLabel          = new QLabel("Product Identifier:", box);
    pTopLayout->addWidget(m_pidLabel);

    m_vidLabel          = new QLabel("Vendor Identifier",   box);
    pTopLayout->addWidget(m_vidLabel);

    return box;
}
