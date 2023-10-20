#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QSerialPortInfo>
#include "SerialView.hpp"
#include "Communication/Model/SerialPortService.hpp"

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SerialView::SerialView(SerialPortService *serialPortService,
                       QWidget *parent)
    : QWidget{parent},
      m_serialPortService(serialPortService)
{
    QGridLayout* pTopLayout = new QGridLayout(this);

    m_infoPort = createSelectPortBox();
    pTopLayout->addWidget(m_infoPort);
    pTopLayout->setContentsMargins(0, 0, 0, 0);

    this->fillPortsInfo();

    connect(m_serialPortService,  &SerialPortService::settingsChanged,       this, &SerialView::settingsModelChanged);
    connect(m_selectPortComboBox, QOverload<int>::of(&QComboBox::activated), this, &SerialView::onNameChanged);


    m_serialPortService->emitState();
}

void SerialView::settingsModelChanged(const SerialPortSettings &settings)
{
    this->setPortName(settings.name);
}

void SerialView::onNameChanged()
{
    QString name = m_selectPortComboBox->currentText();
    m_serialPortService->setName(name);
}

QGroupBox *SerialView::createSelectPortBox()
{
    QGroupBox* box = new QGroupBox(this);
    QVBoxLayout* pTopLayout = new QVBoxLayout(box);

    m_selectPortComboBox = new QComboBox(box);
    pTopLayout->addWidget(m_selectPortComboBox);

    m_descriptionLabel  = new QLabel("Description:",        box);
    pTopLayout->addWidget(m_descriptionLabel);

    m_manufacturerLabel = new QLabel("Manufacturer:",       box);
    pTopLayout->addWidget(m_manufacturerLabel);

    m_serialNumberLabel = new QLabel("Serial Number:",      box);
    pTopLayout->addWidget(m_serialNumberLabel);

    m_locationLabel     = new QLabel("Location:",           box);
    pTopLayout->addWidget(m_locationLabel);

    m_pidLabel          = new QLabel("Product Identifier:", box);
    pTopLayout->addWidget(m_pidLabel);

    m_vidLabel          = new QLabel("Vendor Identifier",   box);
    pTopLayout->addWidget(m_vidLabel);

    return box;
}

void SerialView::fillPortsInfo()
{
    m_selectPortComboBox->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;
    QString systemLocation;

    const auto infos = QSerialPortInfo::availablePorts();

    for (const QSerialPortInfo &info : infos)
    {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        systemLocation = info.systemLocation();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << systemLocation
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

        m_selectPortComboBox->addItem(list.first(), list);
    }
}

void SerialView::setPortName(const QString &name)
{
    int index = m_selectPortComboBox->findText(name);

    if (index < 0)
    {
        index = 0;
        QString nemeInComboBox = m_selectPortComboBox->itemText(index);

        // Combobox has correct port value but service does not,
        // therfore set it to service.
        if (nemeInComboBox.size() > 0) {
            m_serialPortService->setName(nemeInComboBox);
        }
    }

    m_selectPortComboBox->setCurrentIndex(index);
    showPortInfo(index);
}

void SerialView::showPortInfo(int index)
{
    if (index == -1)
        return;

    QStringList list = m_selectPortComboBox->itemData(index).toStringList();
    m_descriptionLabel->setText(tr("Description: %1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));
    m_manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
    m_serialNumberLabel->setText(tr("Serial number: %1").arg(list.count() > 3 ? list.at(3) : tr(blankString)));
    m_locationLabel->setText(tr("Location: %1").arg(list.count() > 4 ? list.at(4) : tr(blankString)));
    m_vidLabel->setText(tr("Vendor Identifier: %1").arg(list.count() > 5 ? list.at(5) : tr(blankString)));
    m_pidLabel->setText(tr("Product Identifier: %1").arg(list.count() > 6 ? list.at(6) : tr(blankString)));
}
