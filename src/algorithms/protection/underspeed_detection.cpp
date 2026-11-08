#include "underspeed_detection.h"

#include "../../repositories/engine_status.h"
#include "../../repositories/engine_mode.h"
#include "../../repositories/configuration.h"
#include "../../repositories/fault_status.h"
#include "../../hal/hal_timer.h"


// ============================================================
// Underspeed Detection Algorithm
// ============================================================
//
// Detects an operational loss of engine speed.
//
// RPM validity is handled independently by RPM Validation.
// A physically valid low RPM value can therefore trigger
// UNDERSPEED without being considered RPM_INVALID.
//
// Relevant only in STABILIZING and RUN.
//
// ============================================================

static uint32_t g_underspeedStartMs = 0U;
static bool g_underspeedTimingActive = false;


void underspeedDetectionInit()
{
    g_underspeedStartMs = 0U;
    g_underspeedTimingActive = false;
}


void underspeedDetectionRun()
{
    const EngineMode mode =
        g_engineMode.currentMode;

    const bool relevantMode =
        (mode == EngineMode::STABILIZING) ||
        (mode == EngineMode::RUN);


    // --------------------------------------------------------
    // Not relevant outside established operating modes
    // --------------------------------------------------------

    if (!relevantMode)
    {
        g_underspeedTimingActive = false;

        g_faultStatus.activeFaults &=
            ~faultMask(FaultId::UNDERSPEED);

        return;
    }


    // --------------------------------------------------------
    // Invalid RPM cannot be used for underspeed detection
    // --------------------------------------------------------

    if (!g_engineStatus.rpmValid)
    {
        g_underspeedTimingActive = false;
        return;
    }


    // --------------------------------------------------------
    // Normal operating region
    // --------------------------------------------------------

    if (g_engineStatus.rpm >=
        g_configuration.protectionLimits.underspeedRpm)
    {
        g_underspeedTimingActive = false;

        g_faultStatus.activeFaults &=
            ~faultMask(FaultId::UNDERSPEED);

        return;
    }


    // --------------------------------------------------------
    // Underspeed condition: confirmation timing
    // --------------------------------------------------------

    if (!g_underspeedTimingActive)
    {
        g_underspeedStartMs =
            halTimerGetMilliseconds();

        g_underspeedTimingActive = true;
        return;
    }


    const uint32_t elapsedMs =
        halTimerGetMilliseconds() -
        g_underspeedStartMs;


    if (elapsedMs >=
        g_configuration.protectionLimits
            .underspeedConfirmationMs)
    {
        g_faultStatus.activeFaults |=
            faultMask(FaultId::UNDERSPEED);
    }
}