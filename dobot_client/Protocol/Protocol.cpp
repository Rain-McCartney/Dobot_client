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
    this->resetReceiveBuffer();
    m_stopReceivingOnSerialPort = false;
    m_stoppedThread = true;
    m_messageReceived = false;
    m_waitingSyncRxResponse = false;
    dbg_syncProcessIdx = 0;
    m_syncReceptionStatus = STATUS_OK;
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

    const int maxReceivedBytes = MaximumLengthInBytes;
    uint8_t receiveMsgBuffer[maxReceivedBytes]= {0};
    memset(&receiveMsgBuffer, '\0', sizeof(maxReceivedBytes));
    int numberOfBytes = 0;

    while (!m_stopReceivingOnSerialPort.load())
    {
        //        // Considering read and write to be independent, not using mutex here
        auto rxStatus = static_cast<int>(m_port->readData(receiveMsgBuffer, maxReceivedBytes, &numberOfBytes));

        if ((numberOfBytes < maxReceivedBytes) && (numberOfBytes > 0))
        {
            // "RetrieveDataFromSerialPort - Number of bytes received: " << numberOfBytes;
        }

        if (rxStatus != static_cast<int>(STATUS_OK))
        {
            // "RetrieveDataFromSerialPort - receive transaction failed";
            // Failed to read so notify clients so they can decide actions to take
            notifySyncResponseStatus(rxStatus);
            continue;
        }

        if (numberOfBytes > 0)
        {
            auto buffer = static_cast<uint8_t*>(receiveMsgBuffer);
            auto byteToWrite = static_cast<size_t>(numberOfBytes);
            size_t written;

            do
            {
                written = m_receiveBuffer->writeReceiveData(buffer, byteToWrite);
                bool wroteZero = (written <= byteToWrite) && (written >= 1) && buffer[written - 1] == ZeroPadding;
                byteToWrite -= written;
                buffer += written;

                // Do we need to stop ?
                if (m_stopReceivingOnSerialPort.load())
                {
                    break;
                }

                if (wroteZero)
                {
                    std::array<uint8_t, MaximumLengthInBytes> message {};
                    message.fill(0);
                    uint8_t* receivedMessageFrame = message.data();
                    int receivedMessageFrameSize = 0;
                    rxStatus = static_cast<RxStatusType>(getReceivedMessage(&receivedMessageFrame,
                                                                            receivedMessageFrameSize));
                    if (rxStatus == STATUS_PACKET_SYNCHRO_LOST)
                    {
                        // Do we need to stop ?
                        if (m_stopReceivingOnSerialPort.load())
                        {
                            break;
                        }

                        // "RetrieveDataFromSerialPort: synchro lost";
                        resetReceiveBuffer();
                    }
                    else
                    {
                        // Do we need to stop ?
                        if (m_stopReceivingOnSerialPort.load())
                        {
                            break;
                        }
                        // ReceiveMessageFrame creates new rxMsgFrame, no need to pre-allocate it
                        MessageSmartPtr receiveMsg; // allocated MessageFrame in ReceiveMessageFrame
                        rxStatus = static_cast<RxStatusType>(receiveMessage(receiveMsg,
                                                                            receivedMessageFrame,
                                                                            receivedMessageFrameSize));
                        switch (rxStatus) {
                        case STATUS_PACKET_CHUNK_INPROGRESS:
                            // Goes to the next RX byte (!)
                            // "Chunk in progress received";
                            break;
                        case STATUS_PACKET_PUSH_FRAME_RECIEVED:
                            // "Push message received";
                            break;
                        case STATUS_PACKET_WRONG_CRC:
                            // "CRC error in packet";
                            break;
                        default:
                            this->notifySyncResponse(rxStatus, receiveMsg);
                            break;
                        }

                        // Do we need to stop ?
                        if (m_stopReceivingOnSerialPort.load())
                        {
                            break;
                        }
                    }
                }
            } while (byteToWrite != 0u);
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
    const uint8_t comID = getAndUpdateComID();
    const uint16_t seqNo = getAndUpdateSequentialNumber();
    const uint8_t packetLength = MinimumLengthInByte + static_cast<uint8_t>(payloadSize);
    const std::vector<uint8_t> vector(payload, payload + payloadSize);

    return std::make_shared<Message>(packetLength,
                                     seqNo,
                                     comID,
                                     messageCode,
                                     vector);
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

retStatusCode Protocol::getReceivedMessage(uint8_t** data, int& dataSize)
{
    const retStatusCode retStatus = STATUS_OK;
    uint32_t receivedMessageSize = 0;
    retStatusCode receiveStatus = m_receiveBuffer->getReceivedMessage(*data,
                                                                      receivedMessageSize,
                                                                      MaximumLengthInBytes);
    if (receiveStatus != STATUS_OK) {
        // "GetReceivedMessage() from circular buffer error. Received message size: " << receivedMessageFrameSize << " input buffer size: " << MaximumFrameLengthInBytes;
        resetReceiveBuffer();
        return STATUS_PACKET_SYNCHRO_LOST;
    }
    // check if the frame size is protocol V2 compatible
    if ((receivedMessageSize < MinimumLengthInByte) || (receivedMessageSize > MaximumLengthInBytes)) {
        // "ReceiveMessageFrame- The received message does not have the correct number of bytes - number of bytes received from GUI: " << receivedMessageFrameSize;
        resetReceiveBuffer();
        return STATUS_PACKET_SYNCHRO_LOST;
    }
    dataSize = receivedMessageSize;
    return retStatus;
}

void Protocol::resetReceiveBuffer()
{
    m_receiveBuffer.reset(new CircularBuffer(circularBufferSize));
}

retStatusCode Protocol::receiveMessage(MessageSmartPtr &message,
                                       uint8_t *receivedMessage,
                                       uint32_t receivedMessageSize)
{
    if (receivedMessageSize <= 0)
    {
        return STATUS_BAD_ARGUMENTS;
    }
    std::vector<uint8_t> receivedMessageCobs;
    receivedMessageCobs.reserve(receivedMessageSize);

    for (uint32_t index = 0; index < receivedMessageSize; ++index)
    {
        receivedMessageCobs.push_back(receivedMessage[index]);
    }

    std::vector<uint8_t> receivedMessageUnCobs(receivedMessageCobs);

    uint32_t receivedMessageSizeUnCobs = receivedMessageSize;
    uint8_t* receivedMessageUnCobsData = receivedMessageUnCobs.data();
    retStatusCode retStatus = decodeWithCobs(receivedMessageUnCobsData,
                                             receivedMessageSizeUnCobs);

    if (retStatus != STATUS_OK)
    {
        return retStatus;
    }

    receivedMessageUnCobs.resize(receivedMessageSizeUnCobs);
    receivedMessageUnCobsData = receivedMessageUnCobs.data();

    uint32_t unCobsExtractedFcs = extractCRC32FromReceivedBuffer(receivedMessageUnCobsData, receivedMessageSizeUnCobs);

    try
    {
        message = std::make_shared<Message>(receivedMessageUnCobsData, receivedMessageSizeUnCobs);
    }
    catch (const std::length_error& ex)
    {
        // trying to assign more byte to payload that the max of uint8_t
        // so we have invalid arguments somehow
        return STATUS_BAD_ARGUMENTS;
    }
    if (!message.get())
    {
        return STATUS_ERROR;
    }

    // in MessageFrame the method ApplyCOBS calculated FCS on un-COBS-ed data
    uint32_t beforeApplyCobsFcsCalculated = message->getCrc32();

    retStatus = validateCRC32(unCobsExtractedFcs, beforeApplyCobsFcsCalculated);

    bool hasCRCError = retStatus != STATUS_OK;

    if (hasCRCError)
    {
        // Here we may have real error that CRC/FCS just detected
        // or we are in the unlucky case where FCS had Zero bytes in CRC and/or COBS messed up CRC calculation ...
        // let's check that the received frame is valid and has correct size first
        const bool validFrame = message.get() != nullptr;
        const bool sameSizeFrame = validFrame && (message->getDataSize() == receivedMessageCobs.size());

        if (sameSizeFrame)
        {
            // let's compare the bytes first
            const size_t first_start_byte_offset = 1;
            const size_t crc_and_EOF_offset = 5;
            const size_t nb_compare_bytes = receivedMessageSizeUnCobs - crc_and_EOF_offset - first_start_byte_offset;
            //            void* bufA = (void*)(received_message_frame_COBS_ed.data() + first_start_byte_offset);
            void* bufB = (void*)(message->getData() + first_start_byte_offset);
            const bool sameFrame = (0 == memcmp(receivedMessageCobs.data() + 1, bufB, nb_compare_bytes));
            if (!sameFrame)
            {
                // The CRC is different because the frame header+payload bytes are different
                // This should never happen unless there is memory corruption in GUI

                // "CRC error - Received distinct frame seems like memory got corrupted !";
                // "A:" << std::endl << Hexdump(bufA, static_cast<unsigned int>(nb_compare_bytes)) << std::flush;
                // "B:" << std::endl << Hexdump(bufB, static_cast<unsigned int>(nb_compare_bytes)) << std::flush;
            }
            else
            {
                // If the bytes are the same it does not mean the CRC is detected error is wrong
                // need to compare CRC COBS encoded values not the decoded ones to be sure
                // first check if CRC was changed by COBS
                uint8_t byte_before_CRC = receivedMessageCobs[receivedMessageCobs.size() - crc_and_EOF_offset - 1];
                if (byte_before_CRC == 0x05u)
                {
                    // COBS indicates that the 4 byte of CRC had not Zero
                    // so it means we can use CRC without decoding COBS
                    // to check it against calculated one
                    uint32_t cobs_ed_extracted_FCS = extractCRC32FromReceivedBuffer(receivedMessageCobs.data(), receivedMessageCobs.size());
                    if (cobs_ed_extracted_FCS != unCobsExtractedFcs)
                    {
                        // "CRC extraction error the CRC should be the same";
                    }
                    else
                    {
                        // valid CRC error
                        // "CRC received is wrong and no zero byte was in it";
                    }
                }
                else
                {
                    // had Zero byte in CRC
                    uint32_t cobs_ed_extracted_FCS = extractCRC32FromReceivedBuffer(receivedMessageCobs.data(), receivedMessageCobs.size());
                    if (cobs_ed_extracted_FCS != unCobsExtractedFcs)
                    {
                        // "CRC extraction error CRC should not be be the same Zero should have been encoded by COBS";
                    }
                    else
                    {
                        // valid CRC error
                        // "CRC received is wrong it had zero byte but it's the same when COBS encoded and decoded";
                    }
                }
            }
        }
        else
        {
            // "CRC error - Received distinct frame size !";
        }
    }
    if (hasCRCError)
    {
        message.reset();
        resetReceiveBuffer();
        return retStatus;
    }
    retStatus = validatePacketLength(receivedMessageUnCobsData, receivedMessageSizeUnCobs);
    if (retStatus != STATUS_OK)
    {
        message.reset();
        return retStatus;
    }

    // Need to stop so we do not need to handle push message
    if (m_stopReceivingOnSerialPort.load())
    {
        return retStatus;
    }
    if (m_stopReceivingOnSerialPort.load())
    {
        return STATUS_OK;
    }
    // if protocol version, packet length and crc are ok, return the received frame
    // if not, it will remain nullptr
    // Debug TIP: memory for messageFrame is allocated here, and is passed to upper layers. Memory is allocated for whole V2 Frame
    return retStatus;
}

void Protocol::notifySyncResponse(retStatusCode rxStatus,
                                  const MessageSmartPtr &receiveMsg)
{
    std::unique_lock<std::mutex> cvLock(m_cvMutex);

    if (m_waitingSyncRxResponse.exchange(false))
    {
        // If it is sync response, fill sync rx msg frame pointer and status to be used by sender
        m_syncRxResponsePtr = receiveMsg;
        m_syncReceptionStatus = rxStatus;
        m_messageReceived = true;
        cvLock.unlock();
        m_cv.notify_one();
    }
    else
    {
        //"Synchronous RX msg received, but no one is expecting it";
        resetReceiveBuffer();
    }
}

void Protocol::notifySyncResponseStatus(retStatusCode rxStatus)
{
    MessageSmartPtr emptyRxMsgFrame;
    notifySyncResponse(rxStatus, emptyRxMsgFrame);
}
