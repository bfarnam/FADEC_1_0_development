#include "overtemperature_detection.h"

#include "../../repositories/engine_status.h"
#include "../../repositories/configuration.h"
#include "../../repositories/fault_status.h"
#include "../../hal/hal_timer.h"

// ============================================================
// Overtemperature Detection Algorithm
// ============================================================

static uint32_t g_overtemperatureStartMs = 0U;
static bool g_overtemperatureTimingActive = false;

void overtemperatureDetectionInit()
{
    g_overtemperatureStartMs = 0U;
    g_overtemperatureTimingActive = false;
}

void overtemperatureDetectionRun()
{
    // An invalid EGT signal cannot be used for
    // overtemperature detection.
    if (!g_engineStatus.egtValid)
    {
        g_overtemperatureTimingActive = false;

     
        return;
    }

    const bool overtemperatureActive =
        faultIsActive(
            g_faultStatus.activeFaults,
            FaultId::OVERTEMPERATURE);

    // --------------------------------------------------------
    // Recovery with hysteresis
    // --------------------------------------------------------
    if (overtemperatureActive)
    {
        if (g_engineStatus.egt <=
            g_configuration.protectionLimits.
                overtemperatureClearEgt)
        {
            g_faultStatus.activeFaults &=
                ~faultMask(FaultId::OVERTEMPERATURE);

            g_overtemperatureTimingActive = false;
        }

        return;
    }

    // --------------------------------------------------------
    // Normal region: no overtemperature condition
    // --------------------------------------------------------
    if (g_engineStatus.egt <=
        g_configuration.engineLimits.maximumEgt)
    {
        g_overtemperatureTimingActive = false;
        return;
    }

    // --------------------------------------------------------
    // Overtemperature condition detected:
    // confirmation timing
    // --------------------------------------------------------
    if (!g_overtemperatureTimingActive)
    {
        g_overtemperatureStartMs =
            halTimerGetMilliseconds();

        g_overtemperatureTimingActive = true;
        return;
    }

    const uint32_t elapsedMs =
        halTimerGetMilliseconds() -
        g_overtemperatureStartMs;

    if (elapsedMs >=
        g_configuration.protectionLimits.
            overtemperatureConfirmationMs)
    {
        g_faultStatus.activeFaults |=
            faultMask(FaultId::OVERTEMPERATURE);
    }
}