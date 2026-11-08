#pragma once

#include <stdint.h>


// ============================================================
// Engine Status Repository
// ============================================================
//
// Contains the current engine operating measurements.
//
// Field ownership:
//   rpm      -> RPM Algorithm
//   rpmValid -> RPM Validation Algorithm
//   egt      -> EGT Algorithm
//   egtValid -> EGT Validation Algorithm
//
// ============================================================

struct EngineStatus
{
    uint32_t rpm;   // Current engine speed [RPM]
    bool rpmValid;  // RPM measurement validity
    uint32_t rpmLastUpdateMs;

    float egt;      // Exhaust Gas Temperature [degC]
    bool egtValid;  // EGT measurement validity
    uint32_t egtLastUpdateMs;
};

// Global repository instance
extern EngineStatus g_engineStatus;