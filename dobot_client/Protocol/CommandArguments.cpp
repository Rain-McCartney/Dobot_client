#include "CommandArguments.hpp"

CommandArguments::CommandArguments(uint16_t messageCode,
                                   std::string commandName,
                                   uint8_t* payload,
                                   uint8_t payloadSize,
                                   uint32_t timemeout,
                                   QObject *parent)
    : QObject{parent},
      m_messageCode(messageCode),
      m_payloadSize(payloadSize),
      m_commandName(std::move(commandName)),
      m_timeout(timemeout)
{
    m_payload = reinterpret_cast<uint8_t*>(payload);
}

uint8_t CommandArguments::getPayloadSize() const
{
    return m_payloadSize;
}

uint8_t *CommandArguments::getPayload() const
{
    return m_payload;
}

uint16_t CommandArguments::getMessageCode() const
{
    return m_messageCode;
}

std::string CommandArguments::getCommandName() const
{
    return m_commandName;
}

uint32_t CommandArguments::getTimeout() const
{
    return m_timeout;
}
