#include "flame_out_detection.h"

#include "../../repositories/engine_status.h"
#include "../../repositories/engine_mode.h"
#include "../../repositories/configuration.h"
#include "../../repositories/fault_status.h"
#include "../../hal/hal_timer.h"

// ============================================================
// Flame-Out Detection Algorithm
// ============================================================

static uint32_t g_flameOutStartMs = 0U;
static bool g_flameOutTimingActive = false;

void flameOutDetectionInit()
{
    g_flameOutStartMs = 0U;
    g_flameOutTimingActive = false;
}

void flameOutDetectionRun()
{
    const EngineMode mode = g_engineMode.currentMode;

    const bool relevantMode =
        (mode == EngineMode::STABILIZING) ||
        (mode == EngineMode::RUN);

    // Flame-out is only evaluated in modes where combustion
    // is expected to be established.
    if (!relevantMode)
    {
        g_flameOutTimingActive = false;

        g_faultStatus.activeFaults &=
            ~faultMask(FaultId::FLAME_OUT);

        return;
    }

    // Both signals must be valid for this detector.
    if (!g_engineStatus.rpmValid ||
        !g_engineStatus.egtValid)
    {
        g_flameOutTimingActive = false;
        return;
    }

    const bool flameOutCondition =
        (g_engineStatus.rpm >=
        g_configuration.protectionLimits.flameOutMinimumRpm) &&
        (g_engineStatus.egt <
        g_configuration.protectionLimits.flameOutEgtThreshold);

    if (!flameOutCondition)
    {
        g_flameOutTimingActive = false;

        g_faultStatus.activeFaults &=
            ~faultMask(FaultId::FLAME_OUT);

        return;
    }

    if (!g_flameOutTimingActive)
    {
        g_flameOutStartMs = halTimerGetMilliseconds();
        g_flameOutTimingActive = true;
        return;
    }

    const uint32_t elapsedMs =
        halTimerGetMilliseconds() - g_flameOutStartMs;

    if (elapsedMs >=
        g_configuration.protectionLimits.flameOutConfirmationMs)
    {
        g_faultStatus.activeFaults |=
            faultMask(FaultId::FLAME_OUT);
    }
}