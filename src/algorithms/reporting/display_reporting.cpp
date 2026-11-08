#include "display_reporting.h"

#include "../../hal/hal_display.h"
#include "../../repositories/engine_status.h"
#include "../../repositories/engine_mode.h"
#include "../../repositories/operator_requests.h"

#include <stdio.h>

static const char* engineModeToText(EngineMode mode)
{
    switch (mode)
    {
        case EngineMode::OFF:
            return "OFF";

        case EngineMode::STARTING:
            return "STARTING";

        case EngineMode::IGNITION:
            return "IGNITION";

        case EngineMode::ACCELERATION:
            return "ACCEL";

        case EngineMode::STABILIZING:
            return "STABIL";

        case EngineMode::RUN:
            return "RUN";

        case EngineMode::SHUTDOWN:
            return "SHUTDOWN";

        case EngineMode::COOLING:
            return "COOLING";

        case EngineMode::FAULT:
            return "FAULT";

        default:
            return "UNKNOWN";
    }
}

void displayReportingInit()
{
    // No additional initialization required.
}

static const char* requestedModeToText(EngineModeRequest mode)
{
    switch (mode)
    {
        case EngineModeRequest::OFF:
            return "OFF";

        case EngineModeRequest::START:
            return "START";

        case EngineModeRequest::RUN:
            return "RUN";

        default:
            return "INVALID";
    }

}



void displayReportingRun()
{
    if (!halDisplayIsAvailable())
    {
        return;
    }

    char line0[22];
    char line1[22];
    char line2[22];
    char line3[22];

    if (g_engineStatus.rpmValid)
    {
        snprintf(
            line0,
            sizeof(line0),
            "RPM: %lu",
            static_cast<unsigned long>(g_engineStatus.rpm)
        );
    }
    else
    {
        snprintf(
            line0,
            sizeof(line0),
            "RPM: INVALID"
        );
    }

    if (g_engineStatus.egtValid)
    {
        snprintf(
            line1,
            sizeof(line1),
            "EGT: %.1f C",
            g_engineStatus.egt
        );
    }
    else
    {
        snprintf(
            line1,
            sizeof(line1),
            "EGT: INVALID"
        );
    }

    //snprintf(
       // line2,
       // sizeof(line2),
       // "MODE: %s",
       // engineModeToText(g_engineMode.currentMode)
    //);
    

snprintf(
    line2,
    sizeof(line2),
    "MODE: %s",
    engineModeToText(
        g_engineMode.currentMode
    )
);

    snprintf(
        line3,
        sizeof(line3),
        "THR: %.0f %%",
        g_operatorRequests.throttle
    );

    halDisplayClear();

    halDisplayPrintLine(0U, line0);
    halDisplayPrintLine(1U, line1);
    halDisplayPrintLine(2U, line2);
    halDisplayPrintLine(3U, line3);

    halDisplayUpdate();
}