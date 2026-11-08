
#include "throttle_input.h"

#include "../../hal/hal_operator_input.h"
#include "../../repositories/operator_requests.h"

// ============================================================
// RC THROTTLE scaling limits
// ============================================================
//
// Measured RC values:
//
// Minimum ≈ 1095 us
// Maximum ≈ 1646 us
//
// These values are mapped linearly to:
//
// 1095 us ->   0 %
// 1646 us -> 100 %
//
// ============================================================

static constexpr uint32_t THROTTLE_MIN_US = 1095U;
static constexpr uint32_t THROTTLE_MAX_US = 1646U;

// ============================================================
// Initialization
// ============================================================

void throttleInputInit()
{
    g_operatorRequests.throttle = 0.0f;
    g_operatorRequests.throttleValid = false;
}

// ============================================================
// Execution
// ============================================================

void throttleInputRun()
{
    HalOperatorInput input;

    halOperatorInputGetLatest(input);

    g_operatorRequests.throttleValid =
        input.throttleValid;

    // Invalid or missing RC signal -> safe throttle.
    if (!input.throttleValid)
    {
        g_operatorRequests.throttle = 0.0f;
        return;
    }

    const uint32_t pulseUs = input.throttlePulseUs;

    // --------------------------------------------------------
    // Lower saturation
    // --------------------------------------------------------

    if (pulseUs <= THROTTLE_MIN_US)
    {
        g_operatorRequests.throttle = 0.0f;
        return;
    }

    // --------------------------------------------------------
    // Upper saturation
    // --------------------------------------------------------

    if (pulseUs >= THROTTLE_MAX_US)
    {
        g_operatorRequests.throttle = 100.0f;
        return;
    }

    // --------------------------------------------------------
    // Linear scaling
    // --------------------------------------------------------

    const float numerator =
        static_cast<float>(pulseUs - THROTTLE_MIN_US);

    const float denominator =
        static_cast<float>(
            THROTTLE_MAX_US - THROTTLE_MIN_US);

    g_operatorRequests.throttle =
        100.0f * numerator / denominator;
}