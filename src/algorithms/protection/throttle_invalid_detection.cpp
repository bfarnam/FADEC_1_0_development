#include "throttle_invalid_detection.h"

#include "../../repositories/operator_requests.h"
#include "../../repositories/fault_status.h"

// ============================================================
// Throttle Invalid Detection Algorithm
// ============================================================

void throttleInvalidDetectionInit()
{
    g_faultStatus.activeFaults &=
        ~faultMask(FaultId::THROTTLE_INVALID);
}

void throttleInvalidDetectionRun()
{
    // --------------------------------------------------------
    // THROTTLE validity is only required when the operator
    // requests active engine operation.
    //
    // While OFF, an absent throttle signal is not considered
    // an active operational fault.
    // --------------------------------------------------------

    if (g_operatorRequests.requestedMode ==
        EngineModeRequest::OFF)
    {
        g_faultStatus.activeFaults &=
            ~faultMask(FaultId::THROTTLE_INVALID);

        return;
    }

    // --------------------------------------------------------
    // Active operation requested.
    // THROTTLE signal must be valid.
    //
    // The HAL has already applied the RC signal timeout,
    // therefore no additional confirmation delay is added here.
    // --------------------------------------------------------

    if (!g_operatorRequests.throttleValid)
    {
        g_faultStatus.activeFaults |=
            faultMask(FaultId::THROTTLE_INVALID);

        return;
    }

    // --------------------------------------------------------
    // Valid THROTTLE signal
    // --------------------------------------------------------

    g_faultStatus.activeFaults &=
        ~faultMask(FaultId::THROTTLE_INVALID);
}