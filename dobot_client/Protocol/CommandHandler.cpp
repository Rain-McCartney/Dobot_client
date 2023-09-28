#include "Errors.hpp"
#include "CommandHandler.hpp"
#include "Protocol/Protocol.hpp"
#include "Protocol/CommandArguments.hpp"

CommandHandler::CommandHandler(Protocol *protocol,
                               QObject *parent)
    : QObject{parent},
      m_protocol(protocol)
{

}

retStatusCode CommandHandler::sendCommandWithReturnResponse(CommandArguments *args,
                                                            uint8_t *bytes,
                                                            size_t expectedSize,
                                                            size_t *returnedSize,
                                                            bool checkSize)
{
    m_protocol->sendMessage(args->getPayload(), args->getPayloadSize());

    return STATUS_OK;
}
