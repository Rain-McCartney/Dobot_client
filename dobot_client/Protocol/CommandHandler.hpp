
#pragma once

#include <QObject>
#include "Errors.hpp"
#include "Protocol/Protocol.hpp"

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
                                                bool checkSize = true);

signals:


private:

    Protocol* m_protocol = nullptr;

    /**
     * @brief statusCheckPayload.
     * @param receiveMessage[in] Receive Message.
     * @returns status value from mcu in payload.
     */
    int32_t statusCheckPayload(const MessageSmartPtr& receiveMessage);
};

