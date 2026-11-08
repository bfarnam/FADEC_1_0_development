#include "scheduler.h"
#include "../hal/hal_timer.h"
#include "../hal/hal_serial.h"
#include "../algorithms/acquisition/rpm.h"
#include "../algorithms/reporting/reporting.h"
#include "../algorithms/validation/rpm_validation.h"
#include "../algorithms/acquisition/egt.h"
#include "../algorithms/validation/egt_validation.h"
#include "../algorithms/acquisition/operator_mode.h"
#include "../algorithms/acquisition/throttle_input.h"
#include "../algorithms/reporting/display_reporting.h"

#include "../algorithms/supervision/state_machine.h"
#include "../algorithms/control/speed_target_generation.h"
#include "../algorithms/control/fuel_control.h"
#include "../algorithms/control/starter_control.h"
#include "../algorithms/control/ignition_control.h"
#include "../algorithms/control/gas_control.h"
#include "../repositories/fault_status.h"
#include "../algorithms/protection/rpm_invalid_detection.h"
#include "../algorithms/protection/egt_invalid_detection.h"
#include "../algorithms/protection/throttle_invalid_detection.h"
#include "../algorithms/protection/fault_classification.h"
#include "../algorithms/protection/overspeed_detection.h"
#include "../algorithms/protection/overtemperature_detection.h"

#include "../algorithms/protection/flame_out_detection.h"
#include "../algorithms/protection/underspeed_detection.h"
#include "../algorithms/protection/start_timeout_detection.h"
#include "../algorithms/protection/ignition_timeout_detection.h"
#include "../algorithms/protection/acceleration_timeout_detection.h"
#include "../algorithms/protection/shutdown_timeout_detection.h"

#include "../algorithms/protection/fault_management.h"
#include "../hal/hal_actuators.h"

static uint32_t g_cycleCount = 0U;
static uint32_t g_lastCycleTimeMs = 0U;
static uint8_t g_egtDivider = 0U;
static uint8_t g_displayDivider = 0U;


void schedulerInit()
{
    g_cycleCount = 0U;
    g_lastCycleTimeMs = halTimerGetMilliseconds();
    g_egtDivider = 0U;
    g_displayDivider = 0U;
}

void schedulerRunCycle()
{
    const uint32_t nowMs = halTimerGetMilliseconds();

    // Execute only when the base period has elapsed.
    if ((uint32_t)(nowMs - g_lastCycleTimeMs) <
        SCHEDULER_PERIOD_MS)
    {
        return;
    }

    // Advance by exactly one scheduler period.
    g_lastCycleTimeMs += SCHEDULER_PERIOD_MS;

    // ========================================================
    // Fixed deterministic execution order
    // ========================================================

    // 1. Acquisition Algorithms
    rpmRun();

    ++g_egtDivider;

    if (g_egtDivider >= 10U)
    {
        g_egtDivider = 0U;
        egtRun();
    }

    operatorModeRun();
    throttleInputRun();

    // 2. Validation Algorithms
    rpmValidationRun();
    egtValidationRun();

    // 3. Protection Algorithms
    rpmInvalidDetectionRun();
    egtInvalidDetectionRun();
    throttleInvalidDetectionRun();
    overspeedDetectionRun();
    overtemperatureDetectionRun();
   
    flameOutDetectionRun();
    underspeedDetectionRun();       // NEW
    startTimeoutDetectionRun();
    ignitionTimeoutDetectionRun();
    accelerationTimeoutDetectionRun();
    shutdownTimeoutDetectionRun();
   

    faultClassificationRun();
    faultManagementRun();

    // 4. Supervision
    stateMachineRun();

    // 5. Control Algorithms
    speedTargetGenerationRun();
    
    fuelControlRun(
    static_cast<float>(SCHEDULER_PERIOD_MS) / 1000.0f);
    
    starterControlRun();
    ignitionControlRun();
    gasControlRun();

    // 6. Apply logical commands to hardware
    halActuatorsApplyCommands();

    // 7. Display reporting
    ++g_displayDivider;

    if (g_displayDivider >= 20U)
    {
        g_displayDivider = 0U;
        displayReportingRun();
    }

    // 8. Serial reporting
    reportingRun();

    ++g_cycleCount;
}

uint32_t schedulerGetCycleCount()
{
    return g_cycleCount;
}
