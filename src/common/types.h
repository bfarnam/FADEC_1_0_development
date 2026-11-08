#pragma once

#include <stdint.h>

// ============================================================
// Operator requested operating mode
// ============================================================

enum class EngineModeRequest : uint8_t
{
    OFF = 0,
    START,
    RUN
};

// ============================================================
// Internal FADEC engine operating state
// ============================================================

enum class EngineMode : uint8_t
{
    OFF = 0,
    STARTING,
    IGNITION,
    ACCELERATION,
    STABILIZING,
    RUN,
    SHUTDOWN,
    COOLING,
    FAULT
};

// ============================================================
// Fault severity
// ============================================================

enum class FaultSeverity : uint8_t
{
    NONE = 0,
    INFO,
    WARNING,
    CRITICAL,
    FATAL
};

// ============================================================
// Fault identifiers
// ============================================================
enum class FaultId : uint32_t
{
    NONE                 = 0U,

    RPM_INVALID          = 1UL << 0,
    EGT_INVALID          = 1UL << 1,

    OVERSPEED            = 1UL << 2,
    OVERTEMPERATURE      = 1UL << 3,

    // bit 4 intentionally unused
   
    FLAME_OUT            = 1UL << 5,

    START_TIMEOUT        = 1UL << 6,
    IGNITION_TIMEOUT     = 1UL << 7,
    ACCELERATION_TIMEOUT = 1UL << 8,
    SHUTDOWN_TIMEOUT     = 1UL << 9,
   
    // bit 10 intentionally unused

    UNDERSPEED           = 1UL << 11,
    THROTTLE_INVALID     = 1UL << 12
   
};



// ============================================================
// Generic validity state
// ============================================================

enum class DataValidity : uint8_t
{
    INVALID = 0,
    VALID
};

inline uint32_t faultMask(FaultId fault)
{
    return static_cast<uint32_t>(fault);
}

inline bool faultIsActive(uint32_t mask, FaultId fault)
{
    return (mask & faultMask(fault)) != 0U;
}