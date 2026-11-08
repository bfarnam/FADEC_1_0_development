
#pragma once
#include <Arduino.h>
#include <stdint.h>

// ============================================================
// FADEC Hardware / Board Configuration
// ============================================================
//
// Contains board-specific hardware assignments.
//
// This file shall contain:
//   - GPIO assignments
//   - SPI / I2C pin assignments
//   - PWM channel assignments
//   - Hardware-specific constants
//
// Application Algorithms shall NOT include this file directly.
// It is intended for HAL modules only.
//
// ============================================================


// ============================================================
// RPM Sensor
// ============================================================
constexpr gpio_num_t PIN_RPM_SENSOR = GPIO_NUM_4;

// ============================================================
// MAX31856 - EGT Thermocouple
// ============================================================

constexpr gpio_num_t PIN_EGT_CS   = GPIO_NUM_5;
constexpr gpio_num_t PIN_SPI_SCK  = GPIO_NUM_18;
constexpr gpio_num_t PIN_SPI_MISO = GPIO_NUM_19;
constexpr gpio_num_t PIN_SPI_MOSI = GPIO_NUM_23;


// ============================================================
// Actuators
// ============================================================

constexpr gpio_num_t PIN_STARTER   = GPIO_NUM_16;
constexpr gpio_num_t PIN_FUEL_PUMP = GPIO_NUM_17;


constexpr gpio_num_t PIN_GAS_VALVE = GPIO_NUM_13;

constexpr gpio_num_t PIN_FUEL_VALVE = GPIO_NUM_14;

constexpr gpio_num_t PIN_GLOW_PLUG = GPIO_NUM_25;


// ============================================================
// Operator Inputs
// ============================================================
constexpr gpio_num_t PIN_RC_MODE = GPIO_NUM_26;

constexpr gpio_num_t PIN_RC_THROTTLE = GPIO_NUM_27;

// ============================================================
// Display OLED I2C
// ============================================================

constexpr gpio_num_t PIN_I2C_SDA = GPIO_NUM_21;
constexpr gpio_num_t PIN_I2C_SCL = GPIO_NUM_22;