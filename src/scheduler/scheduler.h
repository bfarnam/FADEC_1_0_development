#pragma once

#include <stdint.h>

// ============================================================
// Scheduler Configuration
// ============================================================

constexpr uint32_t SCHEDULER_PERIOD_MS = 10U;

// ============================================================
// Scheduler Interface
// ============================================================

void schedulerInit();
void schedulerRunCycle();

uint32_t schedulerGetCycleCount();