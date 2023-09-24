#include <QtConcurrent/QtConcurrent>
#include "Errors.hpp"
#include "Protocol/ProtocolHelper.h"
#include "Protocol/CommandHandler.hpp"
#include "CommunicationServiceData.hpp"
#include "Protocol/CommandArguments.hpp"

CommunicationServiceData::CommunicationServiceData(CommandHandler *comHandler,
                                                   QObject *parent)
    : QObject{parent},
      m_commandHandler(comHandler)
{

}

QFuture<quint32> CommunicationServiceData::sendCommandPing(quint32 sequenceNumber)
{
    auto worker = [this](quint32 sequenceNumber)
    {
        const QString& commandName = "Ping";

        const uint8_t requestPayloadSize = 4;
        uint8_t requestPayload[requestPayloadSize] = {0};

        requestPayload[0] = (sequenceNumber >> 0)  &0xFF;
        requestPayload[1] = (sequenceNumber >> 8)  &0xFF;
        requestPayload[2] = (sequenceNumber >> 16) &0xFF;
        requestPayload[3] = (sequenceNumber >> 24) &0xFF;

        CommandArguments arguments(MessageCodes::MSG_CODE_PCM_SET_MCU_PING,
                                   commandName.toStdString(),
                                   requestPayload,
                                   requestPayloadSize);

        const uint8_t responsePayloadSize = 4;
        uint8_t responsePayload[responsePayloadSize] = {0};

        retStatusCode retStatus = m_commandHandler->sendCommandWithReturnResponse(&arguments,
                                                                                  responsePayload,
                                                                                  responsePayloadSize);

        quint32 value = 0;

        if (retStatus == STATUS_OK)
        {
            value |= (responsePayload[0] << 0) | (responsePayload[1] << 8) |
                    (responsePayload[2] << 16) | (responsePayload[3] << 24);

        }



        return value;
    };


    return QtConcurrent::run(worker, sequenceNumber);
}
