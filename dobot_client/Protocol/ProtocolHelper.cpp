#include "Protocol/ProtocolHelper.h"


void applyCOBS(uint8_t* frame, int frameLength)
{
    for (int i = frameLength - 2; i > 0; i--) {
        frame[i] = frame[i - 1];
    }
    // Counter to the next synchro-byte.
    uint8_t counter = 1;
    uint8_t* currentIndexToWrite = frame;
    // Pointer to the end of frame buffer.
    uint8_t* endOfFrame = frame + frameLength - 1;
    // Write synchro-byte to the end of buffer.
    *endOfFrame = ZeroPadding;
    // Go through message buffer.
    while (++frame <= endOfFrame)
    {
        // Write current counter value instead of synchro-byte.
        if (*frame == ZeroPadding)
        {
            // Write counter.
            *currentIndexToWrite = counter;
            // Change pointer to index to write.
            currentIndexToWrite += counter;
            // Reset counter.
            counter = 1;
        }
        else
        {
            counter++;
        }
    }
}
