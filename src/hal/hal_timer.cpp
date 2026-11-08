#include "hal_timer.h"

#include <Arduino.h>

void halTimerInit()
{
    // No explicit initialization required for millis().
}

uint32_t halTimerGetMilliseconds()
{
    return static_cast<uint32_t>(millis());
}