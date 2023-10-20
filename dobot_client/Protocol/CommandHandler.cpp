#include "Errors.hpp"
#include "CommandHandler.hpp"
#include "Protocol/Message.hpp"
#include "Protocol/CommandArguments.hpp"
#include "LoggingCategories/LoggingCategories.hpp"

CommandHandler::CommandHandler(Protocol *protocol,
                               QObject *parent)
    : QObject{parent},
      m_protocol(protocol)
{
}

retStatusCode CommandHandler::sendCommandWithReturnResponse(CommandArguments *args,
                                                            uint8_t *bytes,
                                                            size_t expectedSize,
                                                            bool checkSize)
{
    const std::string commandName = args->getCommandName();
    MessageSmartPtr sendMessage = m_protocol->createMessage(args->getMessageCode(),
                                                            args->getPayload(),
                                                            args->getPayloadSize());

    if (!sendMessage.get())
    {
        qCritical(logCritical()) << "CommandHandler::sendCommand: "
                                 << QString::fromStdString(commandName)
                                 << "could not allocate send message.";

        return STATUS_ERROR;
    }

    MessageSmartPtr receiveMessage;

    retStatusCode retStatus = m_protocol->SendAndReceiveMessage(sendMessage,
                                                                receiveMessage,
                                                                args->getTimeout());
    if (retStatus == STATUS_OK)
    {
        if (receiveMessage.get())
        {
            if (checkSize && (expectedSize == receiveMessage->getPayloadSize()))
            {
                if (statusCheckPayload(receiveMessage) == static_cast<int32_t>(STATUS_OK))
                {
                    memcpy(bytes, receiveMessage->getPayloadData(), receiveMessage->getPayloadSize());
                }
                else
                {
                    qCritical(logCritical()) << "CommandHandler::sendCommandWithReturnResponse: "
                                             << QString::fromStdString(commandName)
                                             << " status from MCU in payload is not equal STATUS_OK.";
                    return STATUS_ERROR;
                }
            }
            else
            {
                qCritical(logCritical()) << "CommandHandler::sendCommandWithReturnResponse: "
                                         << QString::fromStdString(commandName)
                                         << " failed, expected payload size.";

                return STATUS_PACKET_WRONG_SIZE;
            }
        }
        else
        {
            qCritical(logCritical()) << "CommandHandler::sendCommandWithReturnResponse: "
                                     << QString::fromStdString(commandName)
                                     << " could not allocate received message.";
            return STATUS_ERROR;
        }
    }
    else
    {
        qCritical(logCritical()) << "CommandHandler::sendCommandWithReturnResponse: "
                                 << "failed. return status "
                                 << QString::number(retStatus);
    }

    return retStatus;
}

int32_t CommandHandler::statusCheckPayload(const MessageSmartPtr &receiveMessage)
{
    return receiveMessage->intValueFromPayload();
}
