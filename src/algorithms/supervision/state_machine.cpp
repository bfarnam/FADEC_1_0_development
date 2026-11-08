#include "state_machine.h"

#include "../../repositories/engine_mode.h"
#include "../../repositories/operator_requests.h"
#include "../../repositories/engine_status.h"
#include "../../repositories/configuration.h"
#include "../../repositories/fault_status.h"
#include "../../hal/hal_timer.h"
#include "../../hal/hal_serial.h"


static uint32_t g_stabilizationEntryMs = 0U;
static bool g_stabilizationTimingActive = false;

static constexpr uint32_t STABILIZATION_BAND_RPM = 1000U;
static constexpr uint32_t STABILIZATION_HOLD_MS = 2000U;

static float g_ignitionBaselineEgt = 0.0f;
static bool g_ignitionBaselineCaptured = false;

static uint32_t g_ignitionConfirmStartMs = 0U;
static bool g_ignitionConfirmTimingActive = false;

static constexpr float IGNITION_EGT_RISE_C = 30.0f;
static constexpr uint32_t IGNITION_CONFIRM_MS = 500U;

static uint32_t g_coolingEntryMs = 0U;
static bool g_coolingTimingActive = false;



// ============================================================
// State Machine Algorithm
// ============================================================
//
// Supervisory state machine.
//
// Owns:
//   g_engineMode.currentMode
//
// The State Machine does not generate actuator commands.
// It only determines the authorized operating state.
//
// ============================================================

void stateMachineInit()
{
    g_engineMode.currentMode = EngineMode::OFF;

    g_stabilizationEntryMs = 0U;
    g_stabilizationTimingActive = false;

    g_coolingEntryMs = 0U;
    g_coolingTimingActive = false;

    g_ignitionBaselineEgt = 0.0f;
    g_ignitionBaselineCaptured = false;

    g_ignitionConfirmStartMs = 0U;
    g_ignitionConfirmTimingActive = false;
    
}



void stateMachineRun()
{


// STABILIZING timer is state-local.
// Ensure no stale timing survives after leaving STABILIZING.
if (g_engineMode.currentMode != EngineMode::STABILIZING)
{
    g_stabilizationEntryMs = 0U;
    g_stabilizationTimingActive = false;
}   
// COOLING timer is state-local.
// Ensure no stale timing survives after leaving COOLING.
if (g_engineMode.currentMode != EngineMode::COOLING)
{
    g_coolingEntryMs = 0U;
    g_coolingTimingActive = false;
} 

if (g_engineMode.currentMode != EngineMode::IGNITION)
{
    g_ignitionBaselineEgt = 0.0f;
    g_ignitionBaselineCaptured = false;

    g_ignitionConfirmStartMs = 0U;
    g_ignitionConfirmTimingActive = false;
}


// ----------------------------------------------------
// Global FAULT transition
// ----------------------------------------------------
// ----------------------------------------------------
// Global RPM_INVALID protection
// ----------------------------------------------------

const bool rpmInvalidActive =
    faultIsActive(
        g_faultStatus.activeFaults,
        FaultId::RPM_INVALID);   

const bool egtInvalidActive =
    faultIsActive(
        g_faultStatus.activeFaults,
        FaultId::EGT_INVALID);

const bool throttleInvalidActive =
    faultIsActive(
        g_faultStatus.activeFaults,
        FaultId::THROTTLE_INVALID);        

const bool flameOutActive =
    faultIsActive(
        g_faultStatus.activeFaults,
        FaultId::FLAME_OUT);

const bool underspeedActive =
    faultIsActive(
        g_faultStatus.activeFaults,
        FaultId::UNDERSPEED);

const bool hardCriticalFaultActive =
faultIsActive(
    g_faultStatus.activeFaults,
    FaultId::OVERSPEED) ||
faultIsActive(
    g_faultStatus.activeFaults,
    FaultId::OVERTEMPERATURE);

if (hardCriticalFaultActive)
{
    g_engineMode.currentMode =
        EngineMode::FAULT;

    return;
}


    if (rpmInvalidActive)
    {
       
        switch (g_engineMode.currentMode)
        {
        
            case EngineMode::RUN:
            {
                // RPM signal has been lost while combustion may still
                // have been active. Enter the protective cooling sequence.
                g_engineMode.currentMode =
                    EngineMode::COOLING;

                return;
            }    

            case EngineMode::STARTING:
            {
                // No combustion is expected yet.
                // Stop the start sequence if RPM feedback is invalid.
                g_engineMode.currentMode =
                    EngineMode::OFF;

                return;
            }

            case EngineMode::IGNITION:
            case EngineMode::ACCELERATION:
            case EngineMode::STABILIZING:
            case EngineMode::SHUTDOWN:
            {
                // Combustion may be present, or shutdown may already
                // be in progress. Enter the protective cooling sequence.
                g_engineMode.currentMode =
                    EngineMode::COOLING;

                return;
            }

            case EngineMode::COOLING:
            {
                // RPM_INVALID ya ha provocado la transición protectora.
                // Permanecer en COOLING y dejar que la lógica temporizada
                // normal de COOLING determine cuándo pasar a OFF.
                break;
            }
            case EngineMode::OFF:
            {
                // Already in the safest state.
                return;
            }

            default:
                break;
        }
    }
    
    
    
    // ----------------------------------------------------
    // Global EGT_INVALID protection
    // ----------------------------------------------------

    if (egtInvalidActive)
    {
        switch (g_engineMode.currentMode)
        {
            case EngineMode::STARTING:
            {
                // No combustion is expected yet.
                // A valid EGT signal is required before continuing
                // the start sequence.
                g_engineMode.currentMode = EngineMode::OFF;
                return;
            }

            case EngineMode::IGNITION:
            case EngineMode::ACCELERATION:
            {
                // Combustion may already be present.
                // Loss of EGT protection requires a protective
                // transition through COOLING.
                g_engineMode.currentMode = EngineMode::COOLING;
                return;
            }

            case EngineMode::STABILIZING:
            case EngineMode::RUN:
            {
                // Combustion is active and EGT protection
                // is no longer available.
                g_engineMode.currentMode = EngineMode::COOLING;
                return;
            }

            case EngineMode::SHUTDOWN:
            {
                // Fuel shutdown is already in progress.
                // Allow the normal SHUTDOWN logic to continue.
                break;
            }

            case EngineMode::COOLING:
            {
                // Continue normal timed cooling.
                break;
            }

            case EngineMode::OFF:
            {
                // Remain OFF while EGT is invalid.
                // This also prevents a new start.
                return;
            }

            default:
                break;
        }
    }
    // ----------------------------------------------------
    // Global THROTTLE_INVALID protection
    // ----------------------------------------------------

    if (throttleInvalidActive)
    {
        switch (g_engineMode.currentMode)
        {
            case EngineMode::STARTING:
            {
                // No combustion is expected yet.
                // Abort the start sequence.
                g_engineMode.currentMode =
                    EngineMode::OFF;

                return;
            }

            case EngineMode::IGNITION:
            case EngineMode::ACCELERATION:
            {
                // Combustion may already be present.
                // Enter the protective cooling sequence.
                g_engineMode.currentMode =
                    EngineMode::COOLING;

                return;
            }

            case EngineMode::STABILIZING:
            case EngineMode::RUN:
            {
                // Loss of throttle command during normal
                // combustion requires a controlled shutdown.
                g_engineMode.currentMode =
                    EngineMode::SHUTDOWN;

                return;
            }

            case EngineMode::SHUTDOWN:
            {
                // Controlled shutdown is already in progress.
                break;
            }

            case EngineMode::COOLING:
            {
                // Continue normal cooling.
                break;
            }

            case EngineMode::OFF:
            {
                // Remain OFF and prevent a new start while
                // the throttle signal is invalid.
                return;
            }

            case EngineMode::FAULT:
            {
                // FAULT remains latched.
                return;
            }

            default:
                break;
        }
    }

    // ----------------------------------------------------
    // Global FLAME_OUT protection
    // ----------------------------------------------------

    if (flameOutActive)
    {
        switch (g_engineMode.currentMode)
        {
            case EngineMode::STABILIZING:
            case EngineMode::RUN:
            {
                // Combustion has been lost.
                // Enter the protective cooling sequence.
                g_engineMode.currentMode =
                    EngineMode::COOLING;

                return;
            }

            case EngineMode::COOLING:
            {
                // Continue normal timed cooling.
                break;
            }

            case EngineMode::OFF:
            {
                return;
            }

            default:
                break;
        }
    }

    // ----------------------------------------------------
    // Global UNDERSPEED protection
    // ----------------------------------------------------

    if (underspeedActive)
    {
        switch (g_engineMode.currentMode)
        {
            case EngineMode::STABILIZING:
            case EngineMode::RUN:
            {
                g_engineMode.currentMode =
                    EngineMode::COOLING;

                return;
            }

            case EngineMode::COOLING:
            {
                // Continue normal timed cooling.
                break;
            }

            default:
                break;
        }
    }

    if (g_faultStatus.startAbortRequested)
    {
        switch (g_engineMode.currentMode)
        {
            case EngineMode::STARTING:
            {
                // No combustion has started yet.
                g_engineMode.currentMode =
                    EngineMode::OFF;

                return;
            }

            case EngineMode::IGNITION:
            case EngineMode::ACCELERATION:
            {
                // Combustion may already be present.
                // Enter the protective cooling sequence.
                g_engineMode.currentMode =
                    EngineMode::COOLING;

                return;
            }

            default:
                break;
        }
    }
    
    
    switch (g_engineMode.currentMode)
    {
        // ----------------------------------------------------
        // OFF
        // ----------------------------------------------------

        case EngineMode::OFF:
        {
            // A previous start attempt was aborted.
            // Require the operator to return the switch to OFF
            // before another start attempt is allowed.
            if (g_faultStatus.startAbortRequested)
            {
                break;
            }

         if ((g_operatorRequests.requestedMode ==
                EngineModeRequest::START) &&
                !g_faultStatus.restartInhibit)
            {
                g_engineMode.currentMode =
                    EngineMode::STARTING;
            }   

            break;
        }

        // ----------------------------------------------------
        // STARTING
        // ----------------------------------------------------

        case EngineMode::STARTING:
        {
            // Operator cancels start before ignition.
            if (g_operatorRequests.requestedMode ==
                EngineModeRequest::OFF)
            {
                g_engineMode.currentMode =
                    EngineMode::OFF;

                break;
            }

         
            if (g_engineStatus.rpmValid &&
                (g_engineStatus.rpm >=
                g_configuration.engineLimits.ignitionRpm))
            {
                g_engineMode.currentMode =
                    EngineMode::IGNITION;

                break;
            }

            break;
         
        }

        // IGNITION

      case EngineMode::IGNITION:
        {
            // Operator abort
            if (g_operatorRequests.requestedMode ==
                EngineModeRequest::OFF)
            {
                halSerialPrint(
                    "IGNITION -> COOLING by operator request\n");

                g_engineMode.currentMode =
                    EngineMode::COOLING;

                break;
            }

            // Ignition detection requires a valid EGT.
            if (!g_engineStatus.egtValid)
            {
                g_ignitionConfirmTimingActive = false;
                break;
            }

            // Capture EGT reference once, immediately after
            // entering IGNITION.
            if (!g_ignitionBaselineCaptured)
            {
                g_ignitionBaselineEgt =
                    g_engineStatus.egt;

                g_ignitionBaselineCaptured = true;
                g_ignitionConfirmTimingActive = false;

                break;
            }

            const float ignitionThreshold =
                g_ignitionBaselineEgt +
                IGNITION_EGT_RISE_C;

            // Temperature rise not yet sufficient.
            if (g_engineStatus.egt < ignitionThreshold)
            {
                g_ignitionConfirmTimingActive = false;
                break;
            }

            // Required EGT rise has been reached.
            // Start confirmation timing.
            if (!g_ignitionConfirmTimingActive)
            {
                g_ignitionConfirmStartMs =
                    halTimerGetMilliseconds();

                g_ignitionConfirmTimingActive = true;

                break;
            }

            // Confirm that the EGT rise remains present.
            if ((halTimerGetMilliseconds() -
                g_ignitionConfirmStartMs) >=
                IGNITION_CONFIRM_MS)
            {
                g_ignitionConfirmTimingActive = false;

                g_engineMode.currentMode =
                    EngineMode::ACCELERATION;
            }

            break;
        }
        // ACCELERATION

        case EngineMode::ACCELERATION:
        {
            if (g_operatorRequests.requestedMode ==
                EngineModeRequest::OFF)
            {
                halSerialPrint(
                    "ACCELERATION -> COOLING by operator request\n");

                g_engineMode.currentMode =
                    EngineMode::COOLING;

                break;
            }

            if (g_engineStatus.rpmValid &&
                g_engineStatus.egtValid &&
                (g_engineStatus.rpm >=
                g_configuration.engineLimits.stabilizationRpm) &&
                (g_engineStatus.egt >=
                g_configuration.engineLimits.fuelTransferMinimumEgt))
            {
                g_engineMode.currentMode =
                    EngineMode::STABILIZING;

                break;
            }

            break;
        }

        //STABILIZING

       
       
        case EngineMode::STABILIZING:
        {
         if (g_operatorRequests.requestedMode == EngineModeRequest::OFF)
            {
                halSerialPrint("STABILIZING -> SHUTDOWN by operator request\n");

                g_stabilizationTimingActive = false;
                g_engineMode.currentMode = EngineMode::SHUTDOWN;
                break;
            }  

            if (!g_engineStatus.egtValid)
            {
                g_stabilizationTimingActive = false;
                break;
            }

            const uint32_t targetRpm =
                g_configuration.engineLimits.stabilizationRpm;

            const uint32_t lowerLimit =
                (targetRpm > STABILIZATION_BAND_RPM)
                ? targetRpm - STABILIZATION_BAND_RPM
                : 0U;

            const uint32_t upperLimit =
                targetRpm + STABILIZATION_BAND_RPM;

            const bool rpmStable =
                (g_engineStatus.rpm >= lowerLimit) &&
                (g_engineStatus.rpm <= upperLimit);

            const bool egtStable =
                (g_engineStatus.egt >=
                g_configuration.engineLimits.fuelTransferMinimumEgt);

            if (!rpmStable || !egtStable)
            {
                g_stabilizationTimingActive = false;
                break;
            }
            

            if (!g_stabilizationTimingActive)
            {
                g_stabilizationEntryMs =
                    halTimerGetMilliseconds();

                g_stabilizationTimingActive = true;

                break;
            }

            if ((halTimerGetMilliseconds() -
                g_stabilizationEntryMs) >=
                STABILIZATION_HOLD_MS)
            {
                if (g_operatorRequests.requestedMode ==
                    EngineModeRequest::RUN)
                {
                    g_stabilizationTimingActive = false;

                    g_engineMode.currentMode =
                        EngineMode::RUN;
                }
            }

            break;
        }
       
        //RUN

        case EngineMode::RUN:
        {
            if (g_operatorRequests.requestedMode ==
                EngineModeRequest::OFF)
            {
                g_engineMode.currentMode =
                    EngineMode::SHUTDOWN;

                break;
            }

            break;
        }
        
        // SHUTDOWN
        case EngineMode::SHUTDOWN:
        {
            if ((g_engineStatus.rpm <=
                g_configuration.engineLimits.coolingRpm) ||
                faultIsActive(
                    g_faultStatus.activeFaults,
                    FaultId::SHUTDOWN_TIMEOUT))
            {
                g_engineMode.currentMode =
                    EngineMode::COOLING;

                break;
            }

            break;
        }
       
        // COOLING

        case EngineMode::COOLING:
        {
            // Cooling time shall only accumulate while rotor speed
            // is valid and within the range where the cooling
            // starter command is permitted.

            const bool coolingRpmEligible =
                g_engineStatus.rpmValid &&
                (g_engineStatus.rpm <
                g_configuration.engineLimits.starterReduce2Rpm);

            if (!coolingRpmEligible)
            {
                g_coolingEntryMs = 0U;
                g_coolingTimingActive = false;
                break;
            }

            if (!g_coolingTimingActive)
            {
                g_coolingEntryMs =
                    halTimerGetMilliseconds();

                g_coolingTimingActive = true;
                break;
            }

            const uint32_t elapsedMs =
                halTimerGetMilliseconds() -
                g_coolingEntryMs;

            if (elapsedMs >=
                g_configuration.engineLimits.coolingDurationMs)
            {
                g_coolingTimingActive = false;

                g_engineMode.currentMode =
                    EngineMode::OFF;
            }

            break;
}
        // ----------------------------------------------------
        // FAULT
        // ----------------------------------------------------

        case EngineMode::FAULT:
        {
            // V1 recovery policy:
            // FAULT is latched for the current operating session.
            // Recovery requires a software restart / new session.
            break;
        }
        // ----------------------------------------------------
        // Default protection
        // ----------------------------------------------------

        default:
        {
            g_engineMode.currentMode =
                EngineMode::OFF;

            break;
        }
    }
}
    
