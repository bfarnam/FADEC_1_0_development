#pragma once

#include <stdint.h>

// ============================================================
// HAL Timer Service
// ============================================================
//
// Provides the application with a hardware-independent
// millisecond time base.
//
// ============================================================

void halTimerInit();

uint32_t halTimerGetMilliseconds();