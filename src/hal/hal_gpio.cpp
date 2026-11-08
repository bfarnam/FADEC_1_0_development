#include "hal_gpio.h"
#include "hal_board_config.h"

#include <Arduino.h>

void halGpioInit()
{
    pinMode(PIN_GAS_VALVE, OUTPUT);
    pinMode(PIN_FUEL_VALVE, OUTPUT);

    // Safe startup state
    digitalWrite(PIN_GAS_VALVE, LOW);
    digitalWrite(PIN_FUEL_VALVE, LOW);

    pinMode(PIN_GLOW_PLUG, OUTPUT);
    digitalWrite(PIN_GLOW_PLUG, LOW);
   
}

void halGasValveWrite(bool open)
{
    digitalWrite(
        PIN_GAS_VALVE,
        open ? HIGH : LOW
    );
}

void halFuelValveWrite(bool open)
{
    digitalWrite(
        PIN_FUEL_VALVE,
        open ? HIGH : LOW
    );
}

void halGlowPlugWrite(bool on)
{
    digitalWrite(
        PIN_GLOW_PLUG,
        on ? HIGH : LOW
    );
}

