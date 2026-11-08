#include "rpm_invalid_detection.h"

#include "../../repositories/engine_status.h"
#include "../../repositories/fault_status.h"
#include "../../repositories/engine_mode.h"



// ============================================================
// RPM Invalid Detection Algorithm
// ============================================================

void rpmInvalidDetectionInit()
{
    g_faultStatus.activeFaults &=
        ~faultMask(FaultId::RPM_INVALID);
}

void rpmInvalidDetectionRun()
{
    const bool rpmInvalid =
        !g_engineStatus.rpmValid ||
        ((g_engineMode.currentMode == EngineMode::RUN) &&
         (g_engineStatus.rpm == 0U));

    if (rpmInvalid)
    {
        g_faultStatus.activeFaults |=
            faultMask(FaultId::RPM_INVALID);

        return;
    }

    g_faultStatus.activeFaults &=
        ~faultMask(FaultId::RPM_INVALID);
}