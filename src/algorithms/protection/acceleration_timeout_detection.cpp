#include "acceleration_timeout_detection.h"

#include "../../repositories/engine_mode.h"
#include "../../repositories/configuration.h"
#include "../../repositories/fault_status.h"
#include "../../hal/hal_timer.h"

// ============================================================
// Acceleration Timeout Detection Algorithm
// ============================================================

static uint32_t g_accelerationEntryMs = 0U;
static bool g_accelerationTimingActive = false;

void accelerationTimeoutDetectionInit()
{
    g_accelerationEntryMs = 0U;
    g_accelerationTimingActive = false;
}

void accelerationTimeoutDetectionRun()
{
    if (g_engineMode.currentMode != EngineMode::ACCELERATION)
    {
        g_accelerationTimingActive = false;

        g_faultStatus.activeFaults &=
            ~faultMask(FaultId::ACCELERATION_TIMEOUT);

        return;
    }

    if (!g_accelerationTimingActive)
    {
        g_accelerationEntryMs = halTimerGetMilliseconds();
        g_accelerationTimingActive = true;
        return;
    }

    const uint32_t elapsedMs =
        halTimerGetMilliseconds() - g_accelerationEntryMs;

    if (elapsedMs >=
        g_configuration.engineLimits.accelerationTimeoutMs)
    {
        g_faultStatus.activeFaults |=
            faultMask(FaultId::ACCELERATION_TIMEOUT);
    }
}