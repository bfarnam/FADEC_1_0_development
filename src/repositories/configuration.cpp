#include "configuration.h"





ConfigurationRepository g_configuration =
{
    // --------------------------------------------------------
    // Engine limits
    // --------------------------------------------------------
    {
        34000U,    // idleRpm
        120000U,    // maximumRpm
        12000U,    // coolingRpm, DEVELOPMENT VALUE

     
        3000U,    // ignitionRpm, DEVELOPMENT VALUE
       
        34000U,    // stabilizationRpm, DEVELOPMENT VALUE
        6000U,   // starterReduce1Rpm - DEVELOPMENT VALUE
        12000U,   // starterReduce2Rpm - DEVELOPMENT VALUE
        16000U,   // starterCutoffRpm  - DEVELOPMENT VALUE


      
        800.0f,  // maximumEgt
        150.0f,  // fuelTransferMinimumEgt - DEVELOPMENT VALUE

        10000U,    // starterTimeoutMs   -DEVELOPMENT VALUE
        60000U,    // ignitionTimeoutMs
        20000U,    // accelerationTimeoutMs DEVELOPMENT VALUE
        10000U,    // shutdownTimeoutMs
        5000U     // coolingDurationMs, DEVELOPMENT VALUE

            },

        
 
    // --------------------------------------------------------
    // Protection limits
    // --------------------------------------------------------
    {
        50U,     // overspeedConfirmationMs - DEVELOPMENT VALUE
        200U,    // overtemperatureConfirmationMs - DEVELOPMENT VALUE
        1000U,    // flameOutConfirmationMs
        1000U,     // underspeedConfirmationMs - DEVELOPMENT VALUE

       
        500U,    // egtSignalTimeoutMs
        500U,    // rpmZeroTimeoutMs - DEVELOPMENT VALUE
            
        5000U,   // maximumRpmDrop - DEVELOPMENT VALUE

        150000U,    // maximumValidRpm - DEVELOPMENT VALUE

        100.0f,   // flameOutEgtThreshold - DEVELOPMENT VALUE
        200U,   // flameOutMinimumRpm - DEVELOPMENT VALUE

        -50.0f,  // minimumValidEgt
        1200.0f,   // maximumValidEgt

        126000U,    // overspeedRpm - DEVELOPMENT VALUE
        120000U,    // overspeedClearRpm - DEVELOPMENT VALUE
        30000U,     // underspeedRpm - DEVELOPMENT VALUE
        750.0f,   // overtemperatureClearEgt - DEVE
    },

    // --------------------------------------------------------
    // Control parameters
    // --------------------------------------------------------
    {
        20.0f,  // starterMinimumCommand
        30.0f,  // starterIntermediateCommand
        40.0f,  // starterMaximumCommand
        30.0f,  // starterCoolingCommand

        10.0f,  // fuelPumpMinimumCommand
        80.0f,  // fuelPumpMaximumCommand

        20.0f,  // fuelPumpRiseRate %/s
        40.0f,   // fuelPumpFallRate  %/s
        0.01f   // stabilizationFuelKp - DEVELOPMENT VALUE
    }

};