#include "fault_status.h"

FaultStatus g_faultStatus =
{
    0U,                  // activeFaults
    FaultSeverity::NONE, // highestSeverity
    false,               // faultPresent
    false,               // criticalFaultPresent
    false,               // startAbortRequested
    false,                  // restartInhibit
    0U                   // sessionFaultHistory
};