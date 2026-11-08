#include "starter_control.h"
#include "../../repositories/configuration.h"
#include "../../repositories/engine_commands.h"
#include "../../repositories/engine_mode.h"
#include "../../repositories/engine_status.h"

// ============================================================
// Starter Control Algorithm
// ============================================================
//
// Owns:
//   g_engineCommands.starterCommand
//
// Logical command only.
// No direct hardware access.
//
// ============================================================


void starterControlInit()
{
    g_engineCommands.starterCommand = 0.0f;
  
}

void starterControlRun()
{
    switch (g_engineMode.currentMode)
    {
        case EngineMode::STARTING:
        case EngineMode::IGNITION:
        case EngineMode::ACCELERATION:
        {
            if (!g_engineStatus.rpmValid)
            {
                g_engineCommands.starterCommand = 0.0f;
                break;
            }

            if (g_engineStatus.rpm <
                g_configuration.engineLimits.starterReduce1Rpm)
            {
                g_engineCommands.starterCommand =
                    g_configuration.controlGains.starterMaximumCommand;
            }
            else if (g_engineStatus.rpm <
                    g_configuration.engineLimits.starterReduce2Rpm)
            {
                g_engineCommands.starterCommand =
                    g_configuration.controlGains.starterIntermediateCommand;
            }
            else if (g_engineStatus.rpm <
                    g_configuration.engineLimits.starterCutoffRpm)
            {
                g_engineCommands.starterCommand =
                    g_configuration.controlGains.starterMinimumCommand;
            }
            else
            {
                g_engineCommands.starterCommand = 0.0f;
}

            break;
        } 
       
        case EngineMode::COOLING:
        {
            // Do not energize the starter while rotor speed is
            // unknown or above the range where a 30 % starter
            // command is already permitted during normal start.
            if (!g_engineStatus.rpmValid)
            {
                g_engineCommands.starterCommand = 0.0f;
                break;
            }

            if (g_engineStatus.rpm <
                g_configuration.engineLimits.starterReduce2Rpm)
            {
                g_engineCommands.starterCommand =
                    g_configuration.controlGains.starterCoolingCommand;
            }
            else
            {
                g_engineCommands.starterCommand = 0.0f;
            }

            break;
        }

        case EngineMode::STABILIZING:
        case EngineMode::RUN:
        case EngineMode::OFF:
        case EngineMode::FAULT:

        default:
        {
            g_engineCommands.starterCommand = 0.0f;
           
            break;
        }

    }
}