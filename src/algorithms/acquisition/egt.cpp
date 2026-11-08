#include "egt.h"

#include "../../hal/hal_thermocouple.h"
#include "../../hal/hal_timer.h"
#include "../../repositories/engine_status.h"

// ============================================================
// EGT Acquisition Algorithm
// ============================================================
//
// Acquires exhaust gas temperature through the HAL and
// publishes the latest successfully acquired measurement.
//
// Owns:
//   g_engineStatus.egt
//   g_engineStatus.egtLastUpdateMs
//
// Does NOT own:
//   g_engineStatus.egtValid
//
// Measurement validity is determined by the
// EGT Validation Algorithm.
//
// ============================================================

void egtInit()
{
    g_engineStatus.egt = 0.0f;
    g_engineStatus.egtLastUpdateMs = 0U;
}

void egtRun()
{
    float temperatureC = 0.0f;
    HalThermocoupleStatus status;

    if (!halThermocoupleRead(temperatureC, status))
    {
        // HAL acquisition failed.
        // Preserve the previous measurement and timestamp.
        return;
    }

    g_engineStatus.egt = temperatureC;
    g_engineStatus.egtLastUpdateMs =
        halTimerGetMilliseconds();
}