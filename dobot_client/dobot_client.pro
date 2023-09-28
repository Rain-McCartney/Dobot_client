QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += serialport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Communication/Data/CommunicationServiceData.cpp \
    Communication/Model/AbstractDevice.cpp \
    Communication/Model/CommunicationService.cpp \
    Communication/Model/SerialConfigStore.cpp \
    Communication/Model/SerialPortService.cpp \
    Communication/Widget/CommunicationView.cpp \
    Communication/Widget/SerialView.cpp \
    Protocol/AbstractPort.cpp \
    Protocol/CalculationCRC.cpp \
    Protocol/CommandArguments.cpp \
    Protocol/CommandHandler.cpp \
    Protocol/Message.cpp \
    Protocol/MessagePayload.cpp \
    Protocol/Protocol.cpp \
    Protocol/ProtocolHelper.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Communication/Data/CommunicationServiceData.hpp \
    Communication/Model/AbstractDevice.hpp \
    Communication/Model/CommunicationService.hpp \
    Communication/Model/SerialConfigStore.hpp \
    Communication/Model/SerialPortService.hpp \
    Communication/Model/SerialPortSettigs.h \
    Communication/Widget/CommunicationView.hpp \
    Communication/Widget/SerialView.hpp \
    Errors.hpp \
    Protocol/AbstractPort.hpp \
    Protocol/CalculationCRC.h \
    Protocol/CommandArguments.hpp \
    Protocol/CommandHandler.hpp \
    Protocol/Message.hpp \
    Protocol/MessagePayload.hpp \
    Protocol/Protocol.hpp \
    Protocol/ProtocolHelper.h \
    mainwindow.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
