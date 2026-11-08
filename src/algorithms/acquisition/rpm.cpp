#include "rpm.h"

#include "../../hal/hal_rpm_capture.h"
#include "../../repositories/engine_status.h"
#include "../../repositories/configuration.h"
#include <stdint.h>
#include "../../hal/hal_timer.h"

// ============================================================
// RPM Acquisition Algorithm
// ============================================================
//
// Converts Hall pulse period into engine speed.
//
// Assumption:
//   1 Hall pulse = 1 revolution
//
// Owns:
//   g_engineStatus.rpm
//
// Does NOT own:
//   g_engineStatus.rpmValid
//
// ============================================================

static uint32_t g_lastValidPulseMs = 0U;

void rpmInit()
{
    g_engineStatus.rpm = 0U;
    g_engineStatus.rpmLastUpdateMs = 0U;
    g_lastValidPulseMs = 0U;
}

void rpmRun()
{
    uint32_t periodUs = 0U;
    const uint32_t nowMs = halTimerGetMilliseconds();

    if (halRpmCaptureGetPeriodUs(periodUs))
    {
        if (periodUs != 0U)
        {
            g_engineStatus.rpm =
                static_cast<uint32_t>(
                    60000000UL / periodUs
                );

            g_engineStatus.rpmLastUpdateMs = nowMs;
            g_lastValidPulseMs = nowMs;
        }

        return;
    }

    // No new Hall pulse period available.
    // Absence of pulses does not mean invalid RPM.
    // After the configured timeout, the best speed estimate is zero.
    if ((g_lastValidPulseMs == 0U) ||
        ((uint32_t)(nowMs - g_lastValidPulseMs) >=
         g_configuration.protectionLimits.rpmZeroTimeoutMs))
    {
        g_engineStatus.rpm = 0U;
        g_engineStatus.rpmLastUpdateMs = nowMs;
    }
}