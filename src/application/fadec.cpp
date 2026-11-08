#include "fadec.h"

#include "../scheduler/scheduler.h"
#include "../hal/hal_timer.h"
#include "../common/version.h"
#include "../hal/hal_serial.h"
#include "../algorithms/reporting/reporting.h"
#include "../hal/hal_rpm_capture.h"
#include "../algorithms/acquisition/rpm.h"
#include "../algorithms/validation/rpm_validation.h"
#include "../hal/hal_thermocouple.h"
#include "../algorithms/acquisition/egt.h"
#include "../algorithms/validation/egt_validation.h"
#include "../hal/hal_operator_input.h"
#include "../algorithms/acquisition/operator_mode.h"
#include "../algorithms/acquisition/throttle_input.h"
#include "../hal/hal_display.h"
#include "../algorithms/reporting/display_reporting.h"

#include "../hal/hal_gpio.h"
#include "../algorithms/supervision/state_machine.h"
#include "../algorithms/control/speed_target_generation.h"
#include "../algorithms/control/fuel_control.h"
#include "../algorithms/control/starter_control.h"
#include "../algorithms/control/ignition_control.h"
#include "../algorithms/control/gas_control.h"
#include "../algorithms/protection/rpm_invalid_detection.h"
#include "../algorithms/protection/egt_invalid_detection.h"
#include "../algorithms/protection/throttle_invalid_detection.h"
#include "../algorithms/protection/overspeed_detection.h"
#include "../algorithms/protection/fault_classification.h"
#include "../algorithms/protection/overtemperature_detection.h"

#include "../algorithms/protection/flame_out_detection.h"
#include "../algorithms/protection/start_timeout_detection.h"
#include "../algorithms/protection/ignition_timeout_detection.h"
#include "../algorithms/protection/acceleration_timeout_detection.h"
#include "../algorithms/protection/shutdown_timeout_detection.h"

#include "../algorithms/protection/fault_management.h"

#include "../algorithms/protection/underspeed_detection.h"

#include "../hal/hal_pwm.h"
#include <esp_system.h>


// ============================================================
// FADEC Application Initialization
// ============================================================

void fadecInit()
{
    // --------------------------------------------------------
    // HAL initialization
    // --------------------------------------------------------
    halGpioInit();
    halPwmInit();
    halTimerInit();
    halSerialInit(115200U);

    const esp_reset_reason_t resetReason =
    esp_reset_reason();

halSerialPrint("ESP32 reset reason: ");

switch (resetReason)
{
    case ESP_RST_POWERON:
        halSerialPrintln("POWERON");
        break;

    case ESP_RST_EXT:
        halSerialPrintln("EXTERNAL");
        break;

    case ESP_RST_SW:
        halSerialPrintln("SOFTWARE");
        break;

    case ESP_RST_PANIC:
        halSerialPrintln("PANIC");
        break;

    case ESP_RST_INT_WDT:
        halSerialPrintln("INT_WDT");
        break;

    case ESP_RST_TASK_WDT:
        halSerialPrintln("TASK_WDT");
        break;

    case ESP_RST_WDT:
        halSerialPrintln("OTHER_WDT");
        break;

    case ESP_RST_DEEPSLEEP:
        halSerialPrintln("DEEPSLEEP");
        break;

    case ESP_RST_BROWNOUT:
        halSerialPrintln("BROWNOUT");
        break;

    case ESP_RST_SDIO:
        halSerialPrintln("SDIO");
        break;

    default:
        halSerialPrintln("UNKNOWN");
        break;
}

    halRpmCaptureInit();
    rpmInit();
    rpmValidationInit();
    halThermocoupleInit();
    halOperatorInputInit();
    reportingInit();
    egtInit();
    egtValidationInit();
    operatorModeInit();
    throttleInputInit();
    stateMachineInit();
    starterControlInit();
    speedTargetGenerationInit();
    fuelControlInit();
    ignitionControlInit();
    gasControlInit();
    rpmInvalidDetectionInit();
    faultClassificationInit();
    egtInvalidDetectionInit();
    throttleInvalidDetectionInit();
    overspeedDetectionInit();
    overtemperatureDetectionInit();
  
    flameOutDetectionInit();
    underspeedDetectionInit();
    startTimeoutDetectionInit();
    ignitionTimeoutDetectionInit();
    accelerationTimeoutDetectionInit();
    shutdownTimeoutDetectionInit();
    
    faultManagementInit();
   
    
    halDisplayInit();
    if (halDisplayIsAvailable())
    {
    halDisplayClear();

    halDisplayPrintLine(0U, "FADEC GR130");
    halDisplayPrintLine(1U,FADEC_SW_VERSION );
    halDisplayPrintLine(2U, "INITIALIZING");

    halDisplayUpdate();
    }
    displayReportingInit();
  


    // --------------------------------------------------------
    // Application infrastructure initialization
    // --------------------------------------------------------

    schedulerInit();
    // --------------------------------------------------------
    // Startup identification
    // --------------------------------------------------------

    halSerialPrintln("");
    halSerialPrintln("================================");
    halSerialPrintln(FADEC_SW_NAME);

    halSerialPrint("Version: ");
    halSerialPrintln(FADEC_SW_VERSION);

    halSerialPrint("Baseline: ");
    halSerialPrintln(FADEC_SW_BASELINE);

    halSerialPrint("Build: ");
    halSerialPrint(FADEC_SW_BUILD_DATE);
    halSerialPrint(" ");
    halSerialPrintln(FADEC_SW_BUILD_TIME);

    halSerialPrintln("FADEC initialized");
    halSerialPrintln("================================");
   
   
}

// ============================================================
// FADEC Application Cyclic Execution
// ============================================================

void fadecRunCycle()
{
    schedulerRunCycle();
}