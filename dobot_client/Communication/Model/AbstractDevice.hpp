
#pragma once

#include <QObject>
#include <QMutex>

class QIODevice;

class AbstractDevice : public QObject
{
    Q_OBJECT
public:
    explicit AbstractDevice(QObject *parent = nullptr);

    int writeData(const uint8_t *data, int size);
    int readData(uint8_t* receiveMessage,
                 int maxNumberOfBytesToReceive,
                 int* numberOfReceivedBytes);

    virtual QString getName() const = 0;

public slots:


    virtual void open()  = 0;
    virtual void close() = 0;

    /**
     * @brief writeSlot
     * Fictitious slot for threads isolation.
    */
    void writeSlot(QByteArray data);

    void onProtReadyRead();


signals:

    /**
     * @brief _writeSignal
     * Fictitious signal for threads isolation.
     */
    void _writeSignal(QByteArray data);

    void opened();
    void closed();

protected:

    QIODevice* m_device;

private:

    static constexpr int s_readBufferSize = 2048;

    QByteArray m_readBuffer;
    QMutex m_readBufferMutex;

};
