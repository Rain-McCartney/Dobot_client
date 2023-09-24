#include "mainwindow.h"
#include "Communication/Model/SerialPortSettigs.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    qRegisterMetaType<SerialPortSettings>("SerialPortSettings");
    return a.exec();
}
