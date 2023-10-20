#include <QtConcurrent/QtConcurrent>

#include "Errors.hpp"
#include "DataHelper.h"
#include "Protocol/ProtocolHelper.h"
#include "CommunicationException.hpp"
#include "Protocol/CommandHandler.hpp"
#include "CommunicationServiceData.hpp"
#include "Protocol/CommandArguments.hpp"
#include "LoggingCategories/LoggingCategories.hpp"

CommunicationServiceData::CommunicationServiceData(CommandHandler *comHandler,
                                                   QObject *parent)
    : QObject{parent},
      m_commandHandler(comHandler)
{

}

QFuture<bool> CommunicationServiceData::sendCommandPing(quint32 sequenceNumber)
{
    auto worker = [this](quint32 sequenceNumber)
    {
        const QString& commandName = "Ping Command";

        std::vector<uint8_t> sendMessage;
        const uint8_t sendMessageSize = 8;
        sendMessage.resize(sendMessageSize);

        const uint32_t type = 0;
        const uint8_t offsetType = 0;
        createPayloadFromUint32(type, offsetType, sendMessage.data());

        const uint8_t offsetSequenceNumber = 4;
        createPayloadFromUint32(sequenceNumber, offsetSequenceNumber, sendMessage.data());

        CommandArguments arguments(MessageCodes::MSG_CODE_PCM_SET_MCU_PING,
                                   commandName.toStdString(),
                                   sendMessage.data(),
                                   sendMessage.size());

        const uint8_t responsePayloadSize = 12;
        //const uint8_t responsePayloadSize = 8;
        uint8_t responsePayload[responsePayloadSize] = {0};

        retStatusCode retStatus = m_commandHandler->sendCommandWithReturnResponse(&arguments,
                                                                                  responsePayload,
                                                                                  responsePayloadSize);
        bool isCorrectConnection = false;

        if (retStatus == STATUS_OK)
        {
            const uint8_t offsetType = 4;
            const uint32_t type = createUint32FromPayload(responsePayload, offsetType);
            //const uint32_t type = 1;

            const uint8_t offsetSequenceNumber = 8;
            const uint32_t seqNumber = createUint32FromPayload(responsePayload, offsetSequenceNumber);
            //const uint32_t seqNumber = sequenceNumber;

            if (type == s_typeResponse && seqNumber == sequenceNumber)
            {
                isCorrectConnection = true;
            }
            else
            {
                isCorrectConnection = false;
            }
        }
        else
        {
            QString msg = commandName + " failed, status "
                    + QString::number(static_cast<uint32_t>(retStatus));

            qCritical(logCritical()) << commandName << " failed";
            throw CommunicationException(msg);
        }
        return isCorrectConnection;
    };
    return QtConcurrent::run(worker, sequenceNumber);
}
