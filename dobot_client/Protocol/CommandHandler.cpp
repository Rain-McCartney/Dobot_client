#include "Errors.hpp"
#include "CommandHandler.hpp"
#include "Protocol/Protocol.hpp"
#include "Protocol/Message.hpp"
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
    MessageSmartPtr sendMessage = m_protocol->createMessage(args->getMessageCode(),
                                                            args->getPayload(),
                                                            args->getPayloadSize());

    if (!sendMessage.get())
    {
//        qCritical(logCritical()) << "CommandHandler::sendCommand: "
//                                 << QString::fromStdString(commandName)
//                                 << "could not allocate send message.";

        return STATUS_ERROR;
    }

    MessageSmartPtr receiveMessage;

    m_protocol->SendAndReceiveMessage(sendMessage, receiveMessage, args->getTimeout());

    return STATUS_OK;
}
