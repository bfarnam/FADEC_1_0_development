#include "engine_status.h"

EngineStatus g_engineStatus =
{
    0U,      // rpm
    false,   // rpmValid
    0U,      // rpmLastUpdateMs
    
    0.0f,    // egt
    false,    // egtValid
    0U       // egtLastUpdateMs
};