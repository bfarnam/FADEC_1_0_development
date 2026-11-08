#include "hal_actuators.h"
#include "hal_gpio.h"
#include "hal_pwm.h"
#include "../repositories/engine_commands.h"

// ============================================================
// Actuator HAL
// ============================================================
//
// Translates logical EngineCommands into physical hardware
// outputs.
//
// ============================================================



void halActuatorsApplyCommands()
{
    halStarterEscWrite(
        g_engineCommands.starterCommand
    );
    halFuelPumpEscWrite(
        g_engineCommands.fuelPumpCommand
    );

    halGasValveWrite(
    g_engineCommands.gasValveCommand
    );
    halFuelValveWrite(
    g_engineCommands.fuelValveCommand
    );

    halGlowPlugWrite(
    g_engineCommands.glowPlugCommand
    );
   

}