
#pragma once

#include <QObject>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <set>
#include <map>
#include <thread>
#include "Protocol/AbstractPort.hpp"

class Protocol : public QObject
{
    Q_OBJECT
public:
    explicit Protocol(QObject *parent = nullptr);

    void setPort(AbstractPort* port);
    void stopReading();

    void initializeReadFunction();


    void retrieveDataFromSerialPort();

    void sendMessage(uint8_t* message, int size);


private:

    // Make non copy-able
    Protocol(const Protocol&) = delete;

    Protocol& operator=(const Protocol&) = delete;

    AbstractPort* m_port = nullptr;

    std::atomic<bool> m_stopReceivingOnSerialPort {};
    std::thread m_retrieveDataFromSerialPort; /**<receive message thread. */
    std::thread::id receiveThreadID; /**< keep thread id of receive thread to identify it */

    std::atomic<bool> m_stoppedThread {}; /**<Parameter Did we thread ? */

    std::mutex m_synchroExchangeMutex; /**<A mutex that protects synchronous TX-RX exchange channel (there is only 1) */


    void stopAndWaitReceiveThreadEnd();

};
