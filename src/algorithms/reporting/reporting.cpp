#include "reporting.h"

#include "../../hal/hal_serial.h"
#include "../../repositories/engine_status.h"
#include "../../repositories/engine_mode.h"
#include "../../repositories/configuration.h"
#include "../../repositories/fault_status.h"
#include "../../hal/hal_thermocouple.h"
#include "../../repositories/operator_requests.h"
#include "../../hal/hal_operator_input.h"
#include "../../repositories/control_targets.h"
#include "../../repositories/engine_commands.h"

static uint32_t g_reportingCounter = 0U;

void reportingInit()
{
    g_reportingCounter = 0U;
}

void reportingRun()
{
    ++g_reportingCounter;

    // Scheduler = 100 Hz
    // Report once every 100 cycles = 1 Hz
    if (g_reportingCounter < 100U)
    {
        return;
    }

    g_reportingCounter = 0U;

    // --------------------------------------------------------
    // RPM
    // --------------------------------------------------------

    halSerialPrint("RPM: ");
    halSerialPrintUint32(g_engineStatus.rpm);

    halSerialPrint(" | RPM valid: ");

    if (g_engineStatus.rpmValid)
    {
        halSerialPrint("YES");
    }
    else
    {
        halSerialPrint("NO");
    }

    // --------------------------------------------------------
    // EGT
    // --------------------------------------------------------

    halSerialPrint(" | EGT: ");
    halSerialPrintFloat(g_engineStatus.egt, 2U);

    halSerialPrint(" | EGT valid: ");
    halSerialPrint(
        g_engineStatus.egtValid ? "YES" : "NO"
    );      
    
    halSerialPrint(" | EGT status: ");
    

    switch (halThermocoupleGetLastStatus())
    {
        case HalThermocoupleStatus::OK:
            halSerialPrint("OK");
            break;

        case HalThermocoupleStatus::NOT_INITIALIZED:
            halSerialPrint("NOT_INIT");
            break;

        case HalThermocoupleStatus::OPEN_CIRCUIT:
            halSerialPrint("OPEN");
            break;

        case HalThermocoupleStatus::OVERVOLTAGE:
            halSerialPrint("OVERVOLT");
            break;

        case HalThermocoupleStatus::THERMOCOUPLE_RANGE:
            halSerialPrint("TC_RANGE");
            break;

        case HalThermocoupleStatus::COLD_JUNCTION_RANGE:
            halSerialPrint("CJ_RANGE");
            break;

        case HalThermocoupleStatus::THERMOCOUPLE_HIGH:
            halSerialPrint("TC_HIGH");
            break;

        case HalThermocoupleStatus::THERMOCOUPLE_LOW:
            halSerialPrint("TC_LOW");
            break;

        case HalThermocoupleStatus::COLD_JUNCTION_HIGH:
            halSerialPrint("CJ_HIGH");
            break;

        case HalThermocoupleStatus::COLD_JUNCTION_LOW:
            halSerialPrint("CJ_LOW");
            break;

        default:
            halSerialPrint("UNKNOWN");
            break;
    }


    // --------------------------------------------------------
    // Operator request
    // --------------------------------------------------------

    halSerialPrint(" | Mode req: ");

    switch (g_operatorRequests.requestedMode)
    {
        case EngineModeRequest::OFF:
            halSerialPrint("OFF");
            break;

        case EngineModeRequest::START:
            halSerialPrint("START");
            break;

        case EngineModeRequest::RUN:
            halSerialPrint("RUN");
            break;

        default:
            halSerialPrint("INVALID");
            break;
    }
    halSerialPrint(" | Throttle: ");
    halSerialPrintFloat(g_operatorRequests.throttle, 1U);
    halSerialPrint(" %");
    HalOperatorInput rcInput;
    halOperatorInputGetLatest(rcInput);

    halSerialPrint(" | RC MODE: ");
    halSerialPrintUint32(rcInput.modePulseUs);
    halSerialPrint(" us / ");

    halSerialPrint(
        rcInput.modeValid ? "YES" : "NO"
    );

    halSerialPrint(" | RC THR: ");
    halSerialPrintUint32(rcInput.throttlePulseUs);

    halSerialPrint(" us / ");
    halSerialPrint(
        rcInput.throttleValid ? "YES" : "NO"
    );

    // --------------------------------------------------------
    // Engine state
    // --------------------------------------------------------

    halSerialPrint(" | State: ");

    switch (g_engineMode.currentMode)
    {
        case EngineMode::OFF:
            halSerialPrint("OFF");
            break;

        case EngineMode::STARTING:
            halSerialPrint("STARTING");
            break;

        case EngineMode::IGNITION:
            halSerialPrint("IGNITION");
            break;

        case EngineMode::ACCELERATION:
            halSerialPrint("ACCELERATION");
            break;

        case EngineMode::STABILIZING:
            halSerialPrint("STABILIZING");
            break;

        case EngineMode::RUN:
            halSerialPrint("RUN");
            break;

        case EngineMode::SHUTDOWN:
            halSerialPrint("SHUTDOWN");
            break;

        case EngineMode::COOLING:
            halSerialPrint("COOLING");
            break;

        case EngineMode::FAULT:
            halSerialPrint("FAULT");
            break;
    }

    halSerialPrint(" | Target RPM: ");
    halSerialPrintUint32(g_controlTargets.targetRpm);

    
    halSerialPrint(" | Pump: ");
    halSerialPrintFloat(g_engineCommands.fuelPumpCommand, 1);
    halSerialPrint(" %");

    halSerialPrint(" | Fuel valve: ");
    halSerialPrint(
        g_engineCommands.fuelValveCommand ? "OPEN" : "CLOSED"
    );


    halSerialPrint(" | Starter: ");
    halSerialPrintFloat(g_engineCommands.starterCommand, 1);
    halSerialPrint("%");

    halSerialPrint(" | Glow: ");
    halSerialPrint(
        g_engineCommands.glowPlugCommand ? "ON" : "OFF"
    );

    halSerialPrint(" | Gas: ");
    halSerialPrint(
        g_engineCommands.gasValveCommand ? "OPEN" : "CLOSED"
    );

    halSerialPrint(" | Fault present: ");
    halSerialPrint(
        g_faultStatus.faultPresent ? "YES" : "NO"
    );

    halSerialPrint(" | Critical fault: ");
    halSerialPrint(
        g_faultStatus.criticalFaultPresent ? "YES" : "NO"
    );

    halSerialPrint(" | Severity: ");
    halSerialPrintUint32(
        static_cast<uint32_t>(g_faultStatus.highestSeverity)
    );

    halSerialPrint(" | Fault history: ");
    halSerialPrintUint32(
        g_faultStatus.sessionFaultHistory);

    // --------------------------------------------------------
    // Faults
    // --------------------------------------------------------

    halSerialPrint(" | Faults: ");
    halSerialPrintUint32(g_faultStatus.activeFaults);

    halSerialPrintln("");


 
}

