#pragma once

// ============================================================
// FADEC Application Interface
// ============================================================
//
// Provides the top-level application entry points.
//
// Responsibilities:
//   - Initialize FADEC application software.
//   - Execute one deterministic application cycle.
//
// The application layer shall not access hardware directly.
// Hardware interaction is performed exclusively through HAL.
//
// ============================================================

void fadecInit();
void fadecRunCycle();