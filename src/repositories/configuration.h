#pragma once

#include <stdint.h>

// ============================================================
// Engine Limits
// ============================================================

struct EngineLimits
{
    // Engine speed limits
    uint32_t idleRpm;
    uint32_t maximumRpm;
    uint32_t coolingRpm;

    // State-machine speed thresholds
   
    uint32_t ignitionRpm;
    uint32_t stabilizationRpm;

    // Starter speed profile thresholds
    uint32_t starterReduce1Rpm;
    uint32_t starterReduce2Rpm;
    uint32_t starterCutoffRpm;

    // Temperature operating limits
    
    float maximumEgt;
    float fuelTransferMinimumEgt;

    // Sequence timeouts
    uint32_t starterTimeoutMs;
    uint32_t ignitionTimeoutMs;
    uint32_t accelerationTimeoutMs;
    uint32_t shutdownTimeoutMs;
    uint32_t coolingDurationMs;
};


// ============================================================
// Protection Limits
// ============================================================

struct ProtectionLimits
{
    // Confirmation times avoid reacting to short transients
    uint32_t overspeedConfirmationMs;
    uint32_t overtemperatureConfirmationMs;
    uint32_t flameOutConfirmationMs;
    uint32_t underspeedConfirmationMs;   // NEW

    

    // Sensor validation
    uint32_t egtSignalTimeoutMs;
    uint32_t rpmZeroTimeoutMs;
    uint32_t maximumRpmDrop;
    uint32_t maximumValidRpm;

    float flameOutEgtThreshold;
    uint32_t flameOutMinimumRpm;

    float minimumValidEgt;
    float maximumValidEgt;

    uint32_t overspeedRpm;
    uint32_t overspeedClearRpm;
    uint32_t underspeedRpm; //NEW
    float overtemperatureClearEgt;
};


// ============================================================
// Control Parameters
// ============================================================

struct ControlGains
{
    
    // Starter command limits/profile
    float starterMinimumCommand;
    float starterIntermediateCommand;
    float starterMaximumCommand;
    float starterCoolingCommand;

    // Fuel pump command limits
    float fuelPumpMinimumCommand;
    float fuelPumpMaximumCommand;

    // Maximum command variation
    float fuelPumpRiseRate;
    float fuelPumpFallRate;
    float stabilizationFuelKp;
};


// ============================================================
// Configuration Repository
// ============================================================
//
// Populated during initialization.
// Read-only during normal FADEC operation.
//
// Field ownership:
//   engineLimits      -> Configuration Algorithm
//   protectionLimits  -> Configuration Algorithm
//   controlGains      -> Configuration Algorithm
//
// ============================================================

struct ConfigurationRepository
{
    EngineLimits engineLimits;
    ProtectionLimits protectionLimits;
    ControlGains controlGains;
};


// Global repository instance
extern ConfigurationRepository g_configuration;