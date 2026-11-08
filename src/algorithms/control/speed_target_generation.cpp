#include "speed_target_generation.h"

#include "../../repositories/control_targets.h"
#include "../../repositories/engine_mode.h"
#include "../../repositories/configuration.h"
#include "../../repositories/operator_requests.h"

void speedTargetGenerationInit()
{
    g_controlTargets.targetRpm = 0U;
}

void speedTargetGenerationRun()
{
    switch (g_engineMode.currentMode)
    {
        case EngineMode::OFF:
        {
            g_controlTargets.targetRpm = 0U;
            break;
        }

        case EngineMode::STARTING:
        {
            g_controlTargets.targetRpm = 0U;
            break;
        }

        case EngineMode::RUN:
        {
            float throttle = g_operatorRequests.throttle;

            if (throttle < 0.0f)
            {
                throttle = 0.0f;
            }
            else if (throttle > 100.0f)
            {
                throttle = 100.0f;
            }

            const uint32_t idleRpm =
                g_configuration.engineLimits.idleRpm;

            const uint32_t maximumRpm =
                g_configuration.engineLimits.maximumRpm;

            const float rpmRange =
                static_cast<float>(maximumRpm - idleRpm);

            g_controlTargets.targetRpm =
                idleRpm +
                static_cast<uint32_t>(
                    rpmRange * throttle / 100.0f
                );

            break;
        }


        case EngineMode::SHUTDOWN:
        {
            g_controlTargets.targetRpm = 0U;
            break;
        }

        case EngineMode::COOLING:
        {
            g_controlTargets.targetRpm =
                g_configuration.engineLimits.coolingRpm;

            break;
        }

        case EngineMode::FAULT:
        {
            g_controlTargets.targetRpm = 0U;
            break;
        }

        default:
        {
            g_controlTargets.targetRpm = 0U;
            break;
        }

        case EngineMode::STABILIZING:
        {
            g_controlTargets.targetRpm =
                g_configuration.engineLimits.stabilizationRpm;

            break;
        }
    }
}