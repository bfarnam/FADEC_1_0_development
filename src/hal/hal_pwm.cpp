#include "hal_pwm.h"
#include "hal_board_config.h"

#include <Arduino.h>

// ============================================================
// ESC PWM HAL
// ============================================================
//
// Standard RC / ESC pulse:
//
//   Frequency : 50 Hz
//   Period    : 20 ms
//   Minimum   : 1000 us
//
// During initialization both ESC outputs are forced to the
// minimum command.
//
// ============================================================

static constexpr uint32_t ESC_PWM_FREQUENCY_HZ = 50U;
static constexpr uint8_t  ESC_PWM_RESOLUTION_BITS = 16U;

static constexpr uint32_t ESC_PERIOD_US = 20000U;
static constexpr uint32_t ESC_MIN_PULSE_US = 1000U;

static uint32_t g_lastStarterDuty = UINT32_MAX;
static uint32_t g_lastPumpDuty = UINT32_MAX;


static uint32_t pulseUsToDuty(uint32_t pulseUs)

{
    constexpr uint32_t MAX_DUTY =
        (1UL << ESC_PWM_RESOLUTION_BITS) - 1UL;

    return (pulseUs * MAX_DUTY) / ESC_PERIOD_US;
}


void halPwmInit()
{
    const uint8_t starterPin =
        static_cast<uint8_t>(PIN_STARTER);

    const uint8_t pumpPin =
        static_cast<uint8_t>(PIN_FUEL_PUMP);

    ledcAttach(
        starterPin,
        ESC_PWM_FREQUENCY_HZ,
        ESC_PWM_RESOLUTION_BITS
    );

    ledcAttach(
        pumpPin,
        ESC_PWM_FREQUENCY_HZ,
        ESC_PWM_RESOLUTION_BITS
    );

    const uint32_t minimumDuty =
        pulseUsToDuty(ESC_MIN_PULSE_US);

    ledcWrite(starterPin, minimumDuty);
    ledcWrite(pumpPin, minimumDuty);

    g_lastStarterDuty = minimumDuty;
    g_lastPumpDuty = minimumDuty;

    
}

static uint32_t commandPercentToPulseUs(float commandPercent)
{
    if (commandPercent < 0.0f)
    {
        commandPercent = 0.0f;
    }

    if (commandPercent > 100.0f)
    {
        commandPercent = 100.0f;
    }

    const float pulseUs =
        1000.0f + (commandPercent * 10.0f);

    return static_cast<uint32_t>(pulseUs);
}


void halStarterEscWrite(float commandPercent)
{
    const uint32_t pulseUs =
        commandPercentToPulseUs(commandPercent);

    const uint32_t duty =
        pulseUsToDuty(pulseUs);

    if (duty == g_lastStarterDuty)
    {
        return;
    }

    ledcWrite(
        static_cast<uint8_t>(PIN_STARTER),
        duty
    );

    g_lastStarterDuty = duty;
}


void halFuelPumpEscWrite(float commandPercent)
{
    const uint32_t pulseUs =
        commandPercentToPulseUs(commandPercent);

    const uint32_t duty =
        pulseUsToDuty(pulseUs);

    if (duty == g_lastPumpDuty)
    {
        return;
    }

    ledcWrite(
        static_cast<uint8_t>(PIN_FUEL_PUMP),
        duty
    );

    g_lastPumpDuty = duty;
}

