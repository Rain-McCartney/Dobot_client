
#pragma once

#include <QObject>

#include "Protocol/ProtocolHelper.h"

class CommandArguments : public QObject
{
    Q_OBJECT
public:
    explicit CommandArguments(uint16_t messageCode,
                              std::string commandName,
                              uint8_t* payload,
                              uint8_t payloadSize,
                              uint32_t timemeout = normalMessageTimeout,
                              QObject *parent = nullptr);

    uint8_t getPayloadSize() const;
    uint8_t* getPayload() const;

    uint16_t getMessageCode() const;
    std::string getCommandName() const;

    uint32_t getTimeout() const;


signals:


private:

    uint16_t m_messageCode;

    uint8_t* m_payload;

    uint8_t m_payloadSize;

    std::string m_commandName;

    uint32_t m_timeout;

};
