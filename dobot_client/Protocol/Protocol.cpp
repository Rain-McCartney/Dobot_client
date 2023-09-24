#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <memory>
#include <utility>
#include "Protocol.hpp"
#include "Errors.hpp"

Protocol::Protocol(QObject *parent)
    : QObject{parent}
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

void Protocol::sendMessage(uint8_t *message, int size)
{
    m_port->writeData(message, size);
}
