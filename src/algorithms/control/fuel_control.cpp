#include "fuel_control.h"

#include "../../repositories/engine_commands.h"
#include "../../repositories/engine_mode.h"
#include "../../repositories/operator_requests.h"
#include "../../repositories/configuration.h"
#include "../../repositories/control_targets.h"
#include "../../repositories/engine_status.h"


// ============================================================
// Fuel Control Algorithm
// ============================================================
//
// Owns:
//   g_engineCommands.fuelPumpCommand
//   g_engineCommands.fuelValveCommand
//
// Logical commands only.
// No direct hardware access.
//
// ============================================================

static float applyPumpRamp(
    float currentCommand,
    float targetCommand,
    float riseRate,
    float fallRate,
    float cycleTimeSeconds)
{
  

    const float maxRisePerCycle =
        riseRate * cycleTimeSeconds;

    const float maxFallPerCycle =
        fallRate * cycleTimeSeconds;

    if (targetCommand > currentCommand)
    {
        const float delta =
            targetCommand - currentCommand;

        if (delta > maxRisePerCycle)
        {
            return currentCommand + maxRisePerCycle;
        }

        return targetCommand;
    }

    const float delta =
        currentCommand - targetCommand;

    if (delta > maxFallPerCycle)
    {
        return currentCommand - maxFallPerCycle;
    }

    return targetCommand;
}

void fuelControlInit()
{
    g_engineCommands.fuelPumpCommand = 0.0f;
    g_engineCommands.fuelValveCommand = false;
}

void fuelControlRun(float cycleTimeSeconds)
{
    switch (g_engineMode.currentMode)
    {
        case EngineMode::OFF:
        {
            g_engineCommands.fuelPumpCommand = 0.0f;
            g_engineCommands.fuelValveCommand = false;

            break;
        }

        case EngineMode::FAULT:
        {
            g_engineCommands.fuelPumpCommand = 0.0f;
            g_engineCommands.fuelValveCommand = false;

            break;
        }

        default:
        {
            // Operating modes without liquid-fuel demand
            // default to a safe fuel-off condition.
            g_engineCommands.fuelPumpCommand = 0.0f;
            g_engineCommands.fuelValveCommand = false;

            break;
        }

        
        case EngineMode::RUN:
        {
            const float minPump =
                g_configuration.controlGains.fuelPumpMinimumCommand;

            const float maxPump =
                g_configuration.controlGains.fuelPumpMaximumCommand;

            const uint32_t idleRpm =
                g_configuration.engineLimits.idleRpm;

            const uint32_t maximumRpm =
                g_configuration.engineLimits.maximumRpm;

            uint32_t targetRpm =
                g_controlTargets.targetRpm;

            if (targetRpm < idleRpm)
            {
                targetRpm = idleRpm;
            }
            else if (targetRpm > maximumRpm)
            {
                targetRpm = maximumRpm;
            }

            float targetPumpCommand = minPump;

            if (maximumRpm > idleRpm)
            {
                targetPumpCommand =
                    minPump +
                    (maxPump - minPump) *
                    static_cast<float>(targetRpm - idleRpm) /
                    static_cast<float>(maximumRpm - idleRpm);
            }

            g_engineCommands.fuelPumpCommand =
                applyPumpRamp(
                    g_engineCommands.fuelPumpCommand,
                    targetPumpCommand,
                    g_configuration.controlGains.fuelPumpRiseRate,
                    g_configuration.controlGains.fuelPumpFallRate,
                    cycleTimeSeconds
                );

            g_engineCommands.fuelValveCommand = true;

            break;
        }

        
      case EngineMode::STABILIZING:
        {
            
            const float minPump =
                g_configuration.controlGains.fuelPumpMinimumCommand;

            const float maxPump =
                g_configuration.controlGains.fuelPumpMaximumCommand;

            const float rpmError =
                static_cast<float>(g_controlTargets.targetRpm) -
                static_cast<float>(g_engineStatus.rpm);

           const float stabilizationKp =
                g_configuration.controlGains.stabilizationFuelKp;

            float targetPumpCommand =
                minPump + stabilizationKp * rpmError;

            if (targetPumpCommand < minPump)
            {
                targetPumpCommand = minPump;
            }
            else if (targetPumpCommand > maxPump)
            {
                targetPumpCommand = maxPump;
            }

           g_engineCommands.fuelPumpCommand =
                applyPumpRamp(
                    g_engineCommands.fuelPumpCommand,
                    targetPumpCommand,
                    g_configuration.controlGains.fuelPumpRiseRate,
                    g_configuration.controlGains.fuelPumpFallRate,
                    cycleTimeSeconds
                );

        

            g_engineCommands.fuelValveCommand = true;

            break;
        }  

        case EngineMode::SHUTDOWN:
            {
                g_engineCommands.fuelPumpCommand = 0.0f;
                g_engineCommands.fuelValveCommand = false;

                break;
            }

        case EngineMode::COOLING:
        {
            g_engineCommands.fuelPumpCommand = 0.0f;
            g_engineCommands.fuelValveCommand = false;

        }   
        
            break;
       
    }
}