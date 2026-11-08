#pragma once

#include <stdint.h>
#include "../common/types.h"

// ============================================================
// Fault Status Repository
// ============================================================
//
// Consolidated FADEC fault status.
//
// Field ownership:
//   activeFaults         -> Fault Detection Algorithms
//   highestSeverity      -> Fault Classification
//   faultPresent         -> Fault Classification
//   criticalFaultPresent -> Fault Classification
//   sessionFaultHistory  -> Fault Management
//   startAbortRequested   -> Fault Management
//
// ============================================================

struct FaultStatus
{
    uint32_t activeFaults;
    FaultSeverity highestSeverity;

    bool faultPresent;
    bool criticalFaultPresent;
    bool startAbortRequested;
    bool restartInhibit;
    uint32_t sessionFaultHistory;
};

// Global repository instance
extern FaultStatus g_faultStatus;