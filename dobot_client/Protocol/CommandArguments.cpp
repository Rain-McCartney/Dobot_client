#include "CommandArguments.hpp"

CommandArguments::CommandArguments(uint16_t messageCode,
                                   std::string commandName,
                                   uint8_t* payload,
                                   uint16_t payloadSize,
                                   QObject *parent)
    : QObject{parent},
      m_messageId(messageCode),
      m_payloadSize(payloadSize),
      m_commandName(std::move(commandName))
{
    m_payload = reinterpret_cast<uint8_t*>(payload);
}

int CommandArguments::getPayloadSize() const
{
    return m_payloadSize;
}

uint8_t *CommandArguments::getPayload() const
{
    return m_payload;
}
