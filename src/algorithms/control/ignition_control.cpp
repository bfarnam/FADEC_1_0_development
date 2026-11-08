#include "ignition_control.h"

#include "../../repositories/engine_commands.h"
#include "../../repositories/engine_mode.h"

// ============================================================
// Ignition Control Algorithm
// ============================================================
//
// Owns:
//   g_engineCommands.glowPlugCommand
//
// Logical command only.
// No direct hardware access.
//
// ============================================================

void ignitionControlInit()
{
    g_engineCommands.glowPlugCommand = false;
}

void ignitionControlRun()
{
    switch (g_engineMode.currentMode)
    {
        case EngineMode::IGNITION:
        case EngineMode::ACCELERATION:
        case EngineMode::STABILIZING:
        {
            g_engineCommands.glowPlugCommand = true;
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
            g_engineCommands.glowPlugCommand = false;
            break;
        }
    }
}