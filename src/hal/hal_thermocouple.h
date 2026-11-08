#pragma once

#include <stdint.h>

// ============================================================
// HAL Thermocouple Service
// ============================================================
//
// Provides hardware-independent access to the MAX31856.
//
// Application Algorithms shall not access SPI or the
// Adafruit_MAX31856 driver directly.
//
// ============================================================

enum class HalThermocoupleStatus : uint8_t
{
    OK = 0,
    NOT_INITIALIZED,
    OPEN_CIRCUIT,
    OVERVOLTAGE,
    THERMOCOUPLE_RANGE,
    COLD_JUNCTION_RANGE,
    THERMOCOUPLE_HIGH,
    THERMOCOUPLE_LOW,
    COLD_JUNCTION_HIGH,
    COLD_JUNCTION_LOW,
    UNKNOWN_FAULT
};

bool halThermocoupleInit();

bool halThermocoupleRead(
    float& temperatureC,
    HalThermocoupleStatus& status
);
HalThermocoupleStatus halThermocoupleGetLastStatus();
bool halThermocoupleReadColdJunction(float& temperatureC);
