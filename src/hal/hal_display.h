#pragma once

#include <stdint.h>

bool halDisplayInit();

void halDisplayClear();

void halDisplayPrintLine(
    uint8_t line,
    const char* text
);

void halDisplayUpdate();

bool halDisplayIsAvailable();