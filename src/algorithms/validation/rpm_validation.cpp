#include "rpm_validation.h"

#include "../../repositories/engine_status.h"
#include "../../repositories/configuration.h"

#include <stdint.h>

// ============================================================
// RPM Validation Algorithm
// ============================================================

static uint32_t g_previousRpm = 0U;


// ============================================================
// Initialization
// ============================================================

void rpmValidationInit()
{
    g_previousRpm = 0U;
    g_engineStatus.rpmValid = true;
}


// ============================================================
// Run
// ============================================================

void rpmValidationRun()
{
    const uint32_t currentRpm = g_engineStatus.rpm;


    // --------------------------------------------------------
    // Maximum physically plausible RPM
    // --------------------------------------------------------

    if (currentRpm >
        g_configuration.protectionLimits.maximumValidRpm)
    {
        g_engineStatus.rpmValid = false;
        g_previousRpm = currentRpm;
        return;
    }




    // --------------------------------------------------------
    // Excessive RPM drop
    // --------------------------------------------------------

    if (g_previousRpm > currentRpm)
    {
        const uint32_t rpmDrop =
            g_previousRpm - currentRpm;

        if (rpmDrop >
            g_configuration.protectionLimits.maximumRpmDrop)
        {
            g_engineStatus.rpmValid = false;
            g_previousRpm = currentRpm;
            return;
        }
    }


    // --------------------------------------------------------
    // Zero RPM is a valid physical condition
    // --------------------------------------------------------

    if (currentRpm == 0U)
    {
        g_engineStatus.rpmValid = true;
        g_previousRpm = 0U;
        return;
    }


    // --------------------------------------------------------
    // Valid RPM
    // --------------------------------------------------------

    g_engineStatus.rpmValid = true;
    g_previousRpm = currentRpm;
}