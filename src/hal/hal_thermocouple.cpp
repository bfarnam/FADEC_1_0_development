#include "hal_thermocouple.h"
#include "hal_board_config.h"

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_MAX31856.h>

// ============================================================
// MAX31856 instance
// ============================================================

static Adafruit_MAX31856 g_sensor(
    static_cast<int8_t>(PIN_EGT_CS),
    &SPI
);

static bool g_initialized = false;

static HalThermocoupleStatus g_lastStatus =
    HalThermocoupleStatus::NOT_INITIALIZED;

// ============================================================
// Fault decoding
// ============================================================

static HalThermocoupleStatus decodeFault(uint8_t fault)
{
    if (fault == 0U)
    {
        return HalThermocoupleStatus::OK;
    }

    if (fault & MAX31856_FAULT_OPEN)
    {
        return HalThermocoupleStatus::OPEN_CIRCUIT;
    }

    if (fault & MAX31856_FAULT_OVUV)
    {
        return HalThermocoupleStatus::OVERVOLTAGE;
    }

    if (fault & MAX31856_FAULT_TCRANGE)
    {
        return HalThermocoupleStatus::THERMOCOUPLE_RANGE;
    }

    if (fault & MAX31856_FAULT_CJRANGE)
    {
        return HalThermocoupleStatus::COLD_JUNCTION_RANGE;
    }

    if (fault & MAX31856_FAULT_TCHIGH)
    {
        return HalThermocoupleStatus::THERMOCOUPLE_HIGH;
    }

    if (fault & MAX31856_FAULT_TCLOW)
    {
        return HalThermocoupleStatus::THERMOCOUPLE_LOW;
    }

    if (fault & MAX31856_FAULT_CJHIGH)
    {
        return HalThermocoupleStatus::COLD_JUNCTION_HIGH;
    }

    if (fault & MAX31856_FAULT_CJLOW)
    {
        return HalThermocoupleStatus::COLD_JUNCTION_LOW;
    }

    return HalThermocoupleStatus::UNKNOWN_FAULT;
}

// ============================================================
// Initialization
// ============================================================

bool halThermocoupleInit()
{
    SPI.begin(
        static_cast<int8_t>(PIN_SPI_SCK),
        static_cast<int8_t>(PIN_SPI_MISO),
        static_cast<int8_t>(PIN_SPI_MOSI),
        static_cast<int8_t>(PIN_EGT_CS)
    );

    if (!g_sensor.begin())
    {
        g_initialized = false;
        g_lastStatus =
            HalThermocoupleStatus::NOT_INITIALIZED;

        return false;
    }

    g_sensor.setThermocoupleType(
        MAX31856_TCTYPE_K
    );

    g_sensor.setConversionMode(
        MAX31856_CONTINUOUS
    );

    g_initialized = true;
    g_lastStatus =
        HalThermocoupleStatus::OK;

    return true;
}

// ============================================================
// Thermocouple read
// ============================================================

bool halThermocoupleRead(
    float& temperatureC,
    HalThermocoupleStatus& status
)
{
    if (!g_initialized)
    {
        status =
            HalThermocoupleStatus::NOT_INITIALIZED;

        g_lastStatus = status;

        return false;
    }

    temperatureC =
        g_sensor.readThermocoupleTemperature();

    const uint8_t fault =
        g_sensor.readFault();

    if (fault != 0U)
    {
        status = decodeFault(fault);
        g_lastStatus = status;

        return false;
    }

    status = HalThermocoupleStatus::OK;
    g_lastStatus = status;

    return true;
}

// ============================================================
// Status
// ============================================================

HalThermocoupleStatus
halThermocoupleGetLastStatus()
{
    return g_lastStatus;
}

// ============================================================
// Cold junction read
// ============================================================

bool halThermocoupleReadColdJunction(
    float& temperatureC
)
{
    if (!g_initialized)
    {
        return false;
    }

    temperatureC =
        g_sensor.readCJTemperature();

    return true;
}