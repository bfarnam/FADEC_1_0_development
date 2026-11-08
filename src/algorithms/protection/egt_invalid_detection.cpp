#include "egt_invalid_detection.h"

#include "../../repositories/engine_status.h"
#include "../../repositories/configuration.h"
#include "../../repositories/fault_status.h"
#include "../../hal/hal_timer.h"

// ============================================================
// EGT Invalid Detection Algorithm
// ============================================================

static uint32_t g_invalidStartMs = 0U;
static bool g_invalidTimingActive = false;

void egtInvalidDetectionInit()
{
    g_invalidStartMs = 0U;
    g_invalidTimingActive = false;

    g_faultStatus.activeFaults &=
        ~faultMask(FaultId::EGT_INVALID);
}

void egtInvalidDetectionRun()
{
    // --------------------------------------------------------
    // Valid EGT
    // --------------------------------------------------------
    if (g_engineStatus.egtValid)
    {
        g_invalidTimingActive = false;

        g_faultStatus.activeFaults &=
            ~faultMask(FaultId::EGT_INVALID);

        return;
    }

    // --------------------------------------------------------
    // No EGT sample has ever been received.
    //
    // During startup allow one signal-timeout period for the
    // thermocouple acquisition chain to provide its first
    // valid sample.
    // --------------------------------------------------------
    if (g_engineStatus.egtLastUpdateMs == 0U)
    {
        if (!g_invalidTimingActive)
        {
            g_invalidStartMs =
                halTimerGetMilliseconds();

            g_invalidTimingActive = true;

            return;
        }

        const uint32_t elapsedMs =
            halTimerGetMilliseconds() -
            g_invalidStartMs;

        if (elapsedMs <
            g_configuration.protectionLimits.egtSignalTimeoutMs)
        {
            return;
        }
    }
    else
    {
        // A sample has existed previously.
        // Validation has already applied the stale-signal
        // timeout, so no additional confirmation is required.
        g_invalidTimingActive = false;
    }

    // --------------------------------------------------------
    // Confirmed invalid EGT
    // --------------------------------------------------------
    g_faultStatus.activeFaults |=
        faultMask(FaultId::EGT_INVALID);
}