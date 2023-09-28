#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <memory>
#include <utility>
#include "Protocol.hpp"
#include "Protocol/ProtocolHelper.h"
#include "Errors.hpp"

Protocol::Protocol(QObject *parent)
    : QObject{parent},
      m_comID(0),
      m_sequenceNumber(0)
{
    m_stopReceivingOnSerialPort = false;
    m_stoppedThread = true;
    //    m_messageReceived = false;
    //    m_waitingSyncRxResponse = false;
    //    dbg_syncProcessIdx = 0;
    //    m_syncReceptionStatus = STATUS_OK;
}

void Protocol::stopAndWaitReceiveThreadEnd()
{
    m_stopReceivingOnSerialPort = true;
    std::thread::id currentThreadID = std::this_thread::get_id();
    if (currentThreadID == this->receiveThreadID)
    {
        // "Dead loop prevented: Trying to wait for thread's end from itself";
        return;
    }

    // If we are not the receive thread we still need to wait for RetrieveDataFromSerialPort to end it's loop with a busy wait
    const size_t busy_milliseconds_wait = 50;

    while (!m_stoppedThread.load())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(busy_milliseconds_wait));
    }

    assert(m_stoppedThread.load());

    // reset received thread id to undefined
    this->receiveThreadID = std::thread::id();
}

void Protocol::setPort(AbstractPort *port)
{
    m_port = port;
}

void Protocol::stopReading()
{
    this->stopAndWaitReceiveThreadEnd();
}

void Protocol::initializeReadFunction()
{
    m_stopReceivingOnSerialPort = false;
    m_retrieveDataFromSerialPort = std::thread(&Protocol::retrieveDataFromSerialPort, std::ref(*this));

    if (m_retrieveDataFromSerialPort.joinable())
    {
        m_retrieveDataFromSerialPort.detach();
    }
}

void Protocol::retrieveDataFromSerialPort()
{
    const int busy_wait_ms = 1;
    m_stoppedThread = false;
    receiveThreadID = std::this_thread::get_id();

    const int maxReceivedBytes = 144;
    uint8_t receiveMsgBuffer[maxReceivedBytes];
    memset(&receiveMsgBuffer, '\0', sizeof(maxReceivedBytes));
    int numberOfBytes = 0;

    while (!m_stopReceivingOnSerialPort.load())
    {
        //        // Considering read and write to be independent, not using mutex here
        auto rxStatus = static_cast<int>(m_port->readData(receiveMsgBuffer, maxReceivedBytes, &numberOfBytes));

        //        if ((numberOfBytes < maxReceivedBytes) && (numberOfBytes > 0))
        //        {
        //            // "RetrieveDataFromSerialPort - Number of bytes received: " << numberOfBytes;
        //        }

        if (rxStatus != static_cast<int>(STATUS_OK))
        {
            // "RetrieveDataFromSerialPort - receive transaction failed";
            // Failed to read so notify clients so they can decide actions to take
            //self.notifySyncResponseStatusWithoutFrame(rxStatus);
            continue;
        }

        if (numberOfBytes > 0)
        {
            auto buffer = static_cast<uint8_t*>(receiveMsgBuffer);
            auto byte_to_write = static_cast<size_t>(numberOfBytes);
            //            size_t written;

            //            do
            //            {
            //                written = self.m_receiveBuffer->WriteReceivedData(buffer, byte_to_write);
            //                bool wroteZero = (written <= byte_to_write) && (written >= 1) && buffer[written - 1] == ZeroPadding;
            //                byte_to_write -= written;
            //                buffer += written;

            //                // Do we need to stop ?
            //                if (self.m_stopReceivingOnSerialPort.load())
            //                {
            //                    break;
            //                }

            //                if (wroteZero)
            //                {
            //                    std::array<uint8_t, MaximumFrameLengthInBytes> message {};
            //                    message.fill(0);
            //                    uint8_t* receivedMessageFrame = message.data();
            //                    int receivedMessageFrameSize = 0;
            //                    rxStatus = static_cast<RxStatusType>(self.GetReceivedMessage(&receivedMessageFrame,
            //                                                                                 receivedMessageFrameSize));
            //                    if (rxStatus == STATUS_PACKET_SYNCHRO_LOST)
            //                    {
            //                        // Do we need to stop ?
            //                        if (self.m_stopReceivingOnSerialPort.load())
            //                        {
            //                            break;
            //                        }

            //                        // "RetrieveDataFromSerialPort: synchro lost";
            //                        self.resetReceiveBuffer();
            //                    }
            //                    else
            //                    {
            //                        // Do we need to stop ?
            //                        if (self.m_stopReceivingOnSerialPort.load())
            //                        {
            //                            break;
            //                        }
            //                        // ReceiveMessageFrame creates new rxMsgFrame, no need to pre-allocate it
            //                        MessageFrameSmartPtr rxMsgFrame; // allocated MessageFrame in ReceiveMessageFrame
            //                        rxStatus = static_cast<RxStatusType>(self.ReceiveMessageFrame(rxMsgFrame,
            //                                                                                      receivedMessageFrame,
            //                                                                                      receivedMessageFrameSize));
            //                        switch (rxStatus) {
            //                        case STATUS_PACKET_CHUNK_INPROGRESS:
            //                            // Goes to the next RX byte (!)
            //                            // "Chunk in progress received";
            //                            break;
            //                        case STATUS_PACKET_PUSH_FRAME_RECIEVED:
            //                            // "Push message received";
            //                            break;
            //                        case STATUS_PACKET_WRONG_CRC:
            //                            // "CRC error in packet";
            //                            break;
            //                        default:
            //                            self.notifySyncResponse(rxStatus, rxMsgFrame);
            //                            break;
            //                        }

            //                        // Do we need to stop ?
            //                        if (self.m_stopReceivingOnSerialPort.load())
            //                        {
            //                            break;
            //                        }
            //                    }
            //                }
            //            } while (byte_to_write != 0u);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(busy_wait_ms));
    }

    //"stopped";
    m_stoppedThread = true;
}

retStatusCode Protocol::SendAndReceiveMessage(const MessageSmartPtr& sendMessage,
                                              MessageSmartPtr& receiveMessage,
                                              uint32_t timeout)
{
    retStatusCode retStatus = transaction(sendMessage, receiveMessage, timeout);

    if (retStatus != STATUS_OK)
    {
        // "SendAndReceiveMessage: Transaction failed";
        return retStatus;
    }
    if (receiveMessage != nullptr)
    {
        if (receiveMessage->getComId() != sendMessage->getComId())
        {
            // "Protocol::SendAndReceiveMessage send ComID " << static_cast<int>(sendMessageFrame->ComID())
            // << " and received ComID " << static_cast<int>(rspMsgFramePtr->ComID())
            // << " are not the same. Abort transactions";

            //this->resetReceiveBuffer();
            return STATUS_PACKET_BAD_MSG_ID;
        }
        if (receiveMessage->getMessageCode() != sendMessage->getMessageCode())
        {
            // "Protocol::SendAndReceiveMessage send MessageCode "
            // << sendMessageFrame->MessageCode() << " and received MessageCode "
            // << rspMsgFramePtr->MessageCode() << " are not the same. Abort transactions";

            //this->resetReceiveBuffer();
            return STATUS_PACKET_WRONG_MSG_CODE_RECEIVED;
        }
        //        receiveMessage = std::make_shared<Message>(); // keep allocated memory in smart pointer

        //        if (receiveMessage.get())
        //        {
        //            receiveMessage->Payload = rspMsgFramePtr->Payload;
        //            // TIP: Pass ownership of the message frame to rMessage
        //            // so rMessage need to be deleted by caller or there is a leak of all message frame
        //            receiveMessage->MessageFrames.push_back(std::move(rspMsgFramePtr));
        //            // NOTE: so rspMsgFramePtr is deleted when rMessage is deleted
        //            // we now use smart pointer so nothing to do
        //        }
    }
    return retStatus;
}

//void Protocol::sendMessage(uint8_t *message, int size)
//{
//    m_port->writeData(message, size);
//}

MessageSmartPtr Protocol::createMessage(uint16_t messageCode,
                                        uint8_t *payload,
                                        uint8_t payloadSize)
{
    MessagePayloadSmartPtr payloadPtr = std::make_shared<MessagePayload>();
    if (payload != nullptr)
    {
        const std::vector<uint8_t> vector(payload, payload + payloadSize);
        payloadPtr->setPayload(vector);
    }
    return createMessageFromPayloadHelper(payloadPtr, payloadSize, messageCode);
}

MessageSmartPtr Protocol::createMessageFromPayloadHelper(const MessagePayloadSmartPtr& payload,
                                                         uint8_t payloadSize,
                                                         uint16_t code)
{
    const uint8_t comID = getAndUpdateComID();
    const uint16_t seqNo = getAndUpdateSequentialNumber();
    const uint8_t packetLength = MinimumLengthInByte + static_cast<uint8_t>(payloadSize);

    return std::make_shared<Message>(packetLength,
                                     seqNo,
                                     comID,
                                     code,
                                     payload->getPayload());
}

uint8_t Protocol::getAndUpdateComID()
{
    const uint8_t comID = m_comID;
    if (m_comID == 255)
    {
        m_comID = 1;
    }
    else
    {
        m_comID++;
    }
    return comID;
}

uint16_t Protocol::getAndUpdateSequentialNumber()
{
    const uint16_t seqNo = m_sequenceNumber;
    if (m_sequenceNumber == 65535)
    {
        m_sequenceNumber = 1;
    }
    else
    {
        m_sequenceNumber++;
    }
    return seqNo;
}

retStatusCode Protocol::transaction(const MessageSmartPtr &sendMessage, MessageSmartPtr &receiveMessage, uint32_t timeout)
{
    // Only one thread can enter synchronous TX-RX process, guard it
    std::unique_lock<std::mutex> lock(m_synchroExchangeMutex);
    ++dbg_syncProcessIdx;

    return sendMessagAndWaitForReception(sendMessage,
                                         receiveMessage,
                                         timeout);
}

retStatusCode Protocol::sendMessagAndWaitForReception(const MessageSmartPtr &sendMessage,
                                                      MessageSmartPtr &receiveMessage,
                                                      uint32_t timeout)
{
    retStatusCode retStatus = STATUS_OK;

    const uint8_t sizeBuffer = sendMessage->getDataSize();

    if (sizeBuffer < MinimumLengthInByte)
    {
        return STATUS_ERROR;
    }

    retStatus = m_port->writeData(sendMessage->getData(), sizeBuffer);

    if (retStatus != STATUS_OK)
    {
        if (retStatus == STATUS_PACKET_FAILED_WRITE_PORT)
        {
            return retStatus;
        }
    }

    return waitForSyncResponse(receiveMessage, timeout);

}

retStatusCode Protocol::waitForSyncResponse(MessageSmartPtr &receiveMessage,
                                            uint32_t timeout_in_milli)
{
    m_waitingSyncRxResponse = true;

    m_messageReceived = false;

    std::unique_lock<std::mutex> cvLock(m_cvMutex);

    const bool waitResult = m_cv.wait_for(cvLock, std::chrono::milliseconds(timeout_in_milli),
                                          [this] { return m_messageReceived.load(); });

    // TIP: m_cvMutex locked again by this thread after wait_for is "unblocked"
    // Get ptr to rx frame and rx status while m_cvMutex is locked - RX thread cannot change it
    receiveMessage = m_syncRxResponsePtr;
    m_waitingSyncRxResponse = false;
    // m_cvMutex is released automatically on cvLock destruction after returns
    auto status = static_cast<retStatusCode>(m_syncReceptionStatus);

    if (!waitResult) {
        //"Read timeout error has occurred";
        return STATUS_PACKET_RECEPTION_TIMEOUT;
    }
    return status;
}
