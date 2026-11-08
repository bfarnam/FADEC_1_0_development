#pragma once

#include "../common/types.h"

// ============================================================
// Operator Requests Repository
// ============================================================
//
// Contains operator intentions only.
// It shall never contain direct actuator commands.
//
// Field ownership:
//   throttle      -> Throttle Input Algorithm
//   throttleValid -> Throttle Input Algorithm
//   requestedMode -> Operator Mode Switch Algorithm
//
// ============================================================

struct OperatorRequests
{
    float throttle;                  // Operator throttle demand: 0.0 ... 100.0 %
    bool throttleValid;              // Valid RC throttle signal available
    EngineModeRequest requestedMode; // OFF / START / RUN
};

// Global repository instance
extern OperatorRequests g_operatorRequests;