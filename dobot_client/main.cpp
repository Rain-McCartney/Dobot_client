#include "mainwindow.h"
#include "Communication/Model/SerialPortSettigs.h"
#include "Communication/Model/CommunicationService.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qInstallMessageHandler(MainWindow::logHandler);
    MainWindow w;
    w.show();

    qRegisterMetaType<InfoDevice>("InfoCommunicationDevice");
    qRegisterMetaType<SerialPortSettings>("SerialPortSettings");
    return a.exec();
}
