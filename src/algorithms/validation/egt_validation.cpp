#include "egt_validation.h"

#include "../../hal/hal_timer.h"
#include "../../repositories/engine_status.h"
#include "../../repositories/configuration.h"

// ============================================================
// Initialization
// ============================================================

void egtValidationInit()
{
    g_engineStatus.egtValid = false;
}

// ============================================================
// Validation
// ============================================================

void egtValidationRun()
{
    const uint32_t nowMs = halTimerGetMilliseconds();

    if (g_engineStatus.egtLastUpdateMs == 0U)
    {
        g_engineStatus.egtValid = false;
        return;
    }

    if ((uint32_t)(nowMs - g_engineStatus.egtLastUpdateMs)
        >= g_configuration.protectionLimits.egtSignalTimeoutMs)
    {
        g_engineStatus.egtValid = false;
        return;
    }

    if ((g_engineStatus.egt
            < g_configuration.protectionLimits.minimumValidEgt) ||
        (g_engineStatus.egt
            > g_configuration.protectionLimits.maximumValidEgt))
    {
        g_engineStatus.egtValid = false;
        return;
    }

    g_engineStatus.egtValid = true;
}