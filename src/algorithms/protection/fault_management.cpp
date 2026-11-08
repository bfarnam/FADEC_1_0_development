#include "fault_management.h"

#include "../../repositories/fault_status.h"
#include "../../repositories/operator_requests.h"

// ============================================================
// Fault Management Algorithm
// ============================================================

void faultManagementInit()
{
    g_faultStatus.startAbortRequested = false;
    g_faultStatus.sessionFaultHistory = 0U;
    g_faultStatus.restartInhibit = false;
}

void faultManagementRun()
{
    const uint32_t startAbortFaults =

        faultMask(FaultId::START_TIMEOUT) |
        faultMask(FaultId::IGNITION_TIMEOUT) |
        faultMask(FaultId::ACCELERATION_TIMEOUT);

    if ((g_faultStatus.activeFaults &
         startAbortFaults) != 0U)
    {
        g_faultStatus.startAbortRequested = true;
    }
    else if (g_operatorRequests.requestedMode ==
             EngineModeRequest::OFF)
    {
        g_faultStatus.startAbortRequested = false;
    }

    // --------------------------------------------------------
    // Restart inhibit after protective shutdown
    // --------------------------------------------------------

    const uint32_t restartInhibitFaults =
    faultMask(FaultId::RPM_INVALID) |
    faultMask(FaultId::EGT_INVALID) |
    faultMask(FaultId::FLAME_OUT) |
    faultMask(FaultId::UNDERSPEED) |
    faultMask(FaultId::THROTTLE_INVALID);

    if ((g_faultStatus.activeFaults &
         restartInhibitFaults) != 0U)
    {
        g_faultStatus.restartInhibit = true;
    }
    else if (g_operatorRequests.requestedMode ==
             EngineModeRequest::OFF)
    {
        g_faultStatus.restartInhibit = false;
    }

    // Session fault history
    g_faultStatus.sessionFaultHistory |=
        g_faultStatus.activeFaults;
}