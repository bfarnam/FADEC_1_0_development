#pragma once

#include "../common/types.h"

// ============================================================
// Engine Mode Repository
// ============================================================
//
// Contains the current FADEC internal operating mode.
//
// Field ownership:
//   currentMode -> State Machine Algorithm
//
// The repository represents the mode authorized by the FADEC,
// not the operator request.
//
// ============================================================

struct EngineModeRepository
{
    EngineMode currentMode;
};

// Global repository instance
extern EngineModeRepository g_engineMode;