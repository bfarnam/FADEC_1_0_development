#include "ignition_timeout_detection.h"

#include "../../repositories/engine_mode.h"
#include "../../repositories/configuration.h"
#include "../../repositories/fault_status.h"
#include "../../hal/hal_timer.h"

// ============================================================
// Ignition Timeout Detection Algorithm
// ============================================================

static uint32_t g_ignitionEntryMs = 0U;
static bool g_ignitionTimingActive = false;

void ignitionTimeoutDetectionInit()
{
    g_ignitionEntryMs = 0U;
    g_ignitionTimingActive = false;
}

void ignitionTimeoutDetectionRun()
{
    if (g_engineMode.currentMode != EngineMode::IGNITION)
    {
        g_ignitionTimingActive = false;

        g_faultStatus.activeFaults &=
            ~faultMask(FaultId::IGNITION_TIMEOUT);

        return;
    }

    if (!g_ignitionTimingActive)
    {
        g_ignitionEntryMs = halTimerGetMilliseconds();
        g_ignitionTimingActive = true;
        return;
    }

    const uint32_t elapsedMs =
        halTimerGetMilliseconds() - g_ignitionEntryMs;

    if (elapsedMs >=
        g_configuration.engineLimits.ignitionTimeoutMs)
    {
        g_faultStatus.activeFaults |=
            faultMask(FaultId::IGNITION_TIMEOUT);
    }
}