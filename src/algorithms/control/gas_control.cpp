#include "gas_control.h"

#include "../../repositories/engine_commands.h"
#include "../../repositories/engine_mode.h"

// ============================================================
// Gas Control Algorithm
// ============================================================
//
// Owns:
//   g_engineCommands.gasValveCommand
//
// Logical command only.
// No direct hardware access.
//
// ============================================================

void gasControlInit()
{
    g_engineCommands.gasValveCommand = false;
}

void gasControlRun()
{
    switch (g_engineMode.currentMode)
    {
        case EngineMode::IGNITION:
        case EngineMode::ACCELERATION:
        case EngineMode::STABILIZING:
        {
            g_engineCommands.gasValveCommand = true;
            break;
        }

        case EngineMode::OFF:
        case EngineMode::STARTING:
        case EngineMode::RUN:
        case EngineMode::SHUTDOWN:
        case EngineMode::COOLING:
        case EngineMode::FAULT:
        default:
        {
            g_engineCommands.gasValveCommand = false;
            break;
        }
    }
}