#include "operator_mode.h"

#include "../../hal/hal_operator_input.h"
#include "../../repositories/operator_requests.h"


// ============================================================
// RC MODE decoding limits
// ============================================================
//
// Measured RC values:
//
// OFF   ≈  966 us
// START ≈ 1519 us
// RUN   ≈ 2072 us
//
// Deliberate dead bands are left between the three valid
// ranges to avoid interpreting intermediate/noisy values.
//
// ============================================================

static constexpr uint32_t MODE_OFF_MIN_US   = 900U;
static constexpr uint32_t MODE_OFF_MAX_US   = 1200U;

static constexpr uint32_t MODE_START_MIN_US = 1300U;
static constexpr uint32_t MODE_START_MAX_US = 1700U;

static constexpr uint32_t MODE_RUN_MIN_US   = 1800U;
static constexpr uint32_t MODE_RUN_MAX_US   = 2100U;

// ============================================================
// Initialization
// ============================================================

void operatorModeInit()
{
    g_operatorRequests.requestedMode =
        EngineModeRequest::OFF;
}

// ============================================================
// Execution
// ============================================================

void operatorModeRun()
{
    HalOperatorInput input;

   halOperatorInputRead(input);

   
    
   if (!input.modeValid)
    {
        g_operatorRequests.requestedMode =
            EngineModeRequest::OFF;

        return;
    } 

    const uint32_t pulseUs = input.modePulseUs;

    // --------------------------------------------------------
    // OFF
    // --------------------------------------------------------

    if ((pulseUs >= MODE_OFF_MIN_US) &&
        (pulseUs <= MODE_OFF_MAX_US))
    {
                    
        g_operatorRequests.requestedMode =
            EngineModeRequest::OFF;

        return;
    }

    // --------------------------------------------------------
    // START
    // --------------------------------------------------------

    if ((pulseUs >= MODE_START_MIN_US) &&
        (pulseUs <= MODE_START_MAX_US))
    {
        g_operatorRequests.requestedMode =
            EngineModeRequest::START;

        return;
    }

    // --------------------------------------------------------
    // RUN
    // --------------------------------------------------------

    if ((pulseUs >= MODE_RUN_MIN_US) &&
        (pulseUs <= MODE_RUN_MAX_US))
    {
        g_operatorRequests.requestedMode =
            EngineModeRequest::RUN;

        return;
    }
// Dead band / intermediate switch transition.
//
// Keep the last valid requested mode.
// A physically invalid or missing RC signal is already
// handled above by forcing OFF.
return;
}