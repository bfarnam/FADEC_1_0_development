#pragma once

#include <stdint.h>

struct ControlTargets
{
    uint32_t targetRpm;
};

extern ControlTargets g_controlTargets;