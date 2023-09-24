
#pragma once

#include <QObject>
#include "Errors.hpp"

class Protocol;
class CommandArguments;

class CommandHandler : public QObject
{
    Q_OBJECT
public:
    explicit CommandHandler(Protocol* protocol,
                            QObject *parent = nullptr);


    retStatusCode sendCommandWithReturnResponse(CommandArguments *args,
                                                uint8_t *bytes,
                                                size_t expectedSize,
                                                size_t *returnedSize = nullptr,
                                                bool checkSize = true);

signals:


private:

    Protocol* m_protocol = nullptr;

};

