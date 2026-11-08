#pragma once

#include <stdint.h>

// ============================================================
// HAL RPM Capture Service
// ============================================================
//
// Captures the time between consecutive Hall sensor pulses.
//
// Application Algorithms shall not access the GPIO or ISR
// directly.
//
// ============================================================

void halRpmCaptureInit();

bool halRpmCaptureGetPeriodUs(uint32_t& periodUs);
uint32_t halRpmCaptureGetPulseCount();
uint32_t halRpmCaptureGetEdgesForLastPeriod();