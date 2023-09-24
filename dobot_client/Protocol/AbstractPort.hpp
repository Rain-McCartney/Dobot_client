
#pragma once

#include <cstdint>

class AbstractPort
{

public:
    explicit AbstractPort();

    virtual int writeData(const uint8_t *data, int size) = 0;

    virtual int readData(uint8_t* receiveMessage,
                         int maxNumberOfBytesToReceive,
                         int* numberOfReceivedBytes) = 0;

};

