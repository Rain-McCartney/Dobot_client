
#pragma once

#include <QObject>
#include <QFuture>

class CommandHandler;

class CommunicationServiceData : public QObject
{
    Q_OBJECT
public:
    explicit CommunicationServiceData(CommandHandler* comHandler,
                                      QObject *parent = nullptr);

    QFuture<bool> sendCommandPing(quint32 sequenceNumber);

signals:


private:

    CommandHandler* m_commandHandler = nullptr;

    static constexpr uint32_t s_typeResponse = 1;

};

