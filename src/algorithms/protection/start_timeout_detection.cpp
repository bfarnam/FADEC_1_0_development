#include "start_timeout_detection.h"

#include "../../repositories/engine_mode.h"
#include "../../repositories/configuration.h"
#include "../../repositories/fault_status.h"
#include "../../hal/hal_timer.h"

// ============================================================
// Start Timeout Detection Algorithm
// ============================================================

static uint32_t g_startEntryMs = 0U;
static bool g_startTimingActive = false;

void startTimeoutDetectionInit()
{
    g_startEntryMs = 0U;
    g_startTimingActive = false;
}

void startTimeoutDetectionRun()
{
    // START_TIMEOUT is only relevant while STARTING.
    if (g_engineMode.currentMode != EngineMode::STARTING)
    {
        g_startTimingActive = false;

        g_faultStatus.activeFaults &=
            ~faultMask(FaultId::START_TIMEOUT);

        return;
    }

    // Start timing when STARTING is first observed.
    if (!g_startTimingActive)
    {
        g_startEntryMs = halTimerGetMilliseconds();
        g_startTimingActive = true;
        return;
    }

    const uint32_t elapsedMs =
        halTimerGetMilliseconds() - g_startEntryMs;

    if (elapsedMs >=
        g_configuration.engineLimits.starterTimeoutMs)
    {
        g_faultStatus.activeFaults |=
            faultMask(FaultId::START_TIMEOUT);
    }
}