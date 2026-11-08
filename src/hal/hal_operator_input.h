#pragma once

#include <stdint.h>
constexpr uint32_t HAL_OPERATOR_INPUT_SIGNAL_TIMEOUT_US =
    100000U;

struct HalOperatorInput
{
    uint32_t throttlePulseUs;
    bool throttleValid;

    uint32_t modePulseUs;
    bool modeValid;
       
};

void halOperatorInputInit();

void halOperatorInputRead(
    HalOperatorInput& input

);
void halOperatorInputGetLatest(
    HalOperatorInput& input
);