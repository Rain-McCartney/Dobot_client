
#pragma once

#include <QObject>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <set>
#include <map>
#include <thread>
#include "Errors.hpp"
#include "Protocol/AbstractPort.hpp"
#include "Protocol/Message.hpp"
#include "Protocol/ProtocolHelper.h"
#include "Protocol/MessagePayload.hpp"

class Protocol : public QObject
{
    Q_OBJECT
public:
    explicit Protocol(QObject *parent = nullptr);

    void setPort(AbstractPort* port);
    void stopReading();

    void initializeReadFunction();


    void retrieveDataFromSerialPort();

    //    void sendMessage(uint8_t* message, int size);

    MessageSmartPtr createMessage(uint16_t messageCode,
                                  uint8_t* payload,
                                  uint8_t payloadSize);


    retStatusCode SendAndReceiveMessage(const MessageSmartPtr &sendMessage,
                                        MessageSmartPtr &receiveMessage,
                                        uint32_t timeout);

private:

    // Make non copy-able
    Protocol(const Protocol&) = delete;

    Protocol& operator=(const Protocol&) = delete;

    AbstractPort* m_port = nullptr;

    uint8_t m_comID; /**< Communication ID (message sequence number).Increments for every message. */

    uint16_t m_sequenceNumber; /**< Frame sequence number. Increments for every message frame. */

    std::atomic<bool> m_stopReceivingOnSerialPort {};

    std::atomic<bool> m_waitingSyncRxResponse {}; /**< Mostly a debug variable to check that someone is expecting sync responce. Guarded by @m_cvMutex */

    using RxStatusType = int; /**< Static casts to hard-coded "int" from enum are useless. This may be used in future to use enum class */

    std::atomic<RxStatusType> m_syncReceptionStatus {}; /**<Reception status to be passes to sync transaction. Guarded by @m_cvMutex*/

    /* Semaphore requires 3 variables.
     * ++ Semaphore group start
     */
    std::condition_variable m_cv; /**<A condition variable used to signal the reception of the message. */

    std::mutex m_cvMutex; /**<A mutex used by CV to guard variable changes */

    std::atomic<bool> m_messageReceived {}; /**<A variable used by m_cv to signal the reception of the message. */

    /* -- Semaphore group end */

    std::thread m_retrieveDataFromSerialPort; /**<receive message thread. */
    std::thread::id receiveThreadID; /**< keep thread id of receive thread to identify it */

    std::atomic<bool> m_stoppedThread {}; /**<Parameter Did we thread ? */

    unsigned int dbg_syncProcessIdx; /**<Debug counter to track synchronous TX-RX processes */

    std::mutex m_synchroExchangeMutex; /**<A mutex that protects synchronous TX-RX exchange channel (there is only 1) */

    const unsigned m_maxNumberOfSendRetries = MaximumLengthInBytes;

    MessageSmartPtr m_syncRxResponsePtr; /**<Pointer to put pointer to synchronous RX MessageFrame object. Guarded by @m_cvMutex*/


    void stopAndWaitReceiveThreadEnd();

    MessageSmartPtr createMessageFromPayloadHelper(const MessagePayloadSmartPtr &payload,
                                                   uint8_t payloadSize,
                                                   uint16_t code);

    uint8_t getAndUpdateComID();

    uint16_t getAndUpdateSequentialNumber();

    retStatusCode transaction(const MessageSmartPtr& sendMessage,
                              MessageSmartPtr& receiveMessage,
                              uint32_t timeout);

    retStatusCode sendMessagAndWaitForReception(const MessageSmartPtr& sendMessage,
                                                MessageSmartPtr& receiveMessage,
                                                uint32_t timeout);

    retStatusCode waitForSyncResponse(MessageSmartPtr& receiveMessage, uint32_t timeout_in_milli);

};
