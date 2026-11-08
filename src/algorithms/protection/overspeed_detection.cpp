#include "overspeed_detection.h"

#include "../../repositories/engine_status.h"
#include "../../repositories/configuration.h"
#include "../../repositories/fault_status.h"
#include "../../hal/hal_timer.h"

// ============================================================
// Overspeed Detection Algorithm
// ============================================================

static uint32_t g_overspeedStartMs = 0U;
static bool g_overspeedTimingActive = false;

void overspeedDetectionInit()
{
    g_overspeedStartMs = 0U;
    g_overspeedTimingActive = false;
}

void overspeedDetectionRun()
{
    // An invalid RPM signal cannot be used for overspeed detection.
    if (!g_engineStatus.rpmValid)
    {
        g_overspeedTimingActive = false;

       

        return;
    }

    const bool overspeedActive =
        faultIsActive(
            g_faultStatus.activeFaults,
            FaultId::OVERSPEED);

    // --------------------------------------------------------
    // Recovery with hysteresis
    // --------------------------------------------------------
    if (overspeedActive)
    {
        if (g_engineStatus.rpm <=
            g_configuration.protectionLimits.overspeedClearRpm)
        {
            g_faultStatus.activeFaults &=
                ~faultMask(FaultId::OVERSPEED);

            g_overspeedTimingActive = false;
        }

        return;
    }

    // --------------------------------------------------------
    // Normal region: no overspeed condition
    // --------------------------------------------------------
    if (g_engineStatus.rpm <=
        g_configuration.protectionLimits.overspeedRpm)
    {
        g_overspeedTimingActive = false;
        return;
    }

    // --------------------------------------------------------
    // Overspeed condition detected: confirmation timing
    // --------------------------------------------------------
    if (!g_overspeedTimingActive)
    {
        g_overspeedStartMs = halTimerGetMilliseconds();
        g_overspeedTimingActive = true;
        return;
    }

    const uint32_t elapsedMs =
        halTimerGetMilliseconds() - g_overspeedStartMs;

    if (elapsedMs >=
        g_configuration.protectionLimits.overspeedConfirmationMs)
    {
        g_faultStatus.activeFaults |=
            faultMask(FaultId::OVERSPEED);
    }
}