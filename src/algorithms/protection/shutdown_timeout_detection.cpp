#include "shutdown_timeout_detection.h"

#include "../../repositories/engine_mode.h"
#include "../../repositories/configuration.h"
#include "../../repositories/fault_status.h"
#include "../../hal/hal_timer.h"

// ============================================================
// Shutdown Timeout Detection Algorithm
// ============================================================

static uint32_t g_shutdownEntryMs = 0U;
static bool g_shutdownTimingActive = false;

void shutdownTimeoutDetectionInit()
{
    g_shutdownEntryMs = 0U;
    g_shutdownTimingActive = false;
}

void shutdownTimeoutDetectionRun()
{
    if (g_engineMode.currentMode != EngineMode::SHUTDOWN)
    {
        g_shutdownTimingActive = false;

        g_faultStatus.activeFaults &=
            ~faultMask(FaultId::SHUTDOWN_TIMEOUT);

        return;
    }

    if (!g_shutdownTimingActive)
    {
        g_shutdownEntryMs = halTimerGetMilliseconds();
        g_shutdownTimingActive = true;
        return;
    }

    const uint32_t elapsedMs =
        halTimerGetMilliseconds() - g_shutdownEntryMs;

    if (elapsedMs >=
        g_configuration.engineLimits.shutdownTimeoutMs)
    {
        g_faultStatus.activeFaults |=
            faultMask(FaultId::SHUTDOWN_TIMEOUT);
    }
}