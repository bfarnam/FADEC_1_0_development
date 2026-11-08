#include "hal_operator_input.h"
#include "hal_board_config.h"
#include <Arduino.h>
#include <esp32-hal-rmt.h>

// ============================================================
// RC Operator Input HAL
// ============================================================
//
// Captures PWM pulse widths from two RC receiver channels:
//
//   MODE     -> GPIO26
//   THROTTLE -> GPIO27
//
// The HAL only measures pulse width and determines whether
// the physical signal is recent and electrically plausible.
//
// Interpretation of MODE and THROTTLE is performed by the
// corresponding Acquisition Algorithms.
//
// ============================================================

static constexpr uint32_t RC_MIN_VALID_PULSE_US = 900U;
static constexpr uint32_t RC_MAX_VALID_PULSE_US = 2100U;




// ------------------------------------------------------------
// MODE channel capture
// ------------------------------------------------------------


static  uint32_t g_modePulseUs = 0U;
static  uint32_t g_modeLastUpdateUs = 0U;

static constexpr uint32_t RMT_FREQUENCY_HZ = 1000000U;

static rmt_data_t g_modeRmtData[
    RMT_SYMBOLS_PER_CHANNEL_BLOCK
];

static size_t g_modeRmtSymbols =
    RMT_SYMBOLS_PER_CHANNEL_BLOCK;

static bool g_modeRmtReceptionActive = false;

static rmt_data_t g_throttleRmtData[
    RMT_SYMBOLS_PER_CHANNEL_BLOCK
];

static size_t g_throttleRmtSymbols =
    RMT_SYMBOLS_PER_CHANNEL_BLOCK;

static bool g_throttleRmtReceptionActive = false;

// ------------------------------------------------------------
// THROTTLE channel capture
// ------------------------------------------------------------

static  uint32_t g_throttlePulseUs = 0U;
static  uint32_t g_throttleLastUpdateUs = 0U;



// ============================================================
// Initialization
// ============================================================

void halOperatorInputInit()
{
    pinMode(PIN_RC_MODE, INPUT);
    pinMode(PIN_RC_THROTTLE, INPUT);

    g_modePulseUs = 0U;
    g_modeLastUpdateUs = 0U;

    g_throttlePulseUs = 0U;
    g_throttleLastUpdateUs = 0U;

    // --------------------------------------------------------
    // MODE RMT initialization
    // --------------------------------------------------------

    const bool modeRmtInitOk =
        rmtInit(
            PIN_RC_MODE,
            RMT_RX_MODE,
            RMT_MEM_NUM_BLOCKS_1,
            RMT_FREQUENCY_HZ
        );

   

    const bool modeRmtMinOk =
        rmtSetRxMinThreshold(
            PIN_RC_MODE,
            0U
        );

    
    const bool modeRmtMaxOk =
        rmtSetRxMaxThreshold(
            PIN_RC_MODE,
            5000U
        );


    g_modeRmtSymbols =
        RMT_SYMBOLS_PER_CHANNEL_BLOCK;

    const bool modeRmtReadOk =
    rmtReadAsync(
        PIN_RC_MODE,
        g_modeRmtData,
        &g_modeRmtSymbols
    );

    g_modeRmtReceptionActive =
        modeRmtInitOk &&
        modeRmtMinOk &&
        modeRmtMaxOk &&
        modeRmtReadOk;

     const bool throttleRmtInitOk =
        rmtInit(
            PIN_RC_THROTTLE,
            RMT_RX_MODE,
            RMT_MEM_NUM_BLOCKS_1,
            RMT_FREQUENCY_HZ
        );

    const bool throttleRmtMinOk =
        rmtSetRxMinThreshold(
            PIN_RC_THROTTLE,
            0U
        );

    const bool throttleRmtMaxOk =
        rmtSetRxMaxThreshold(
            PIN_RC_THROTTLE,
            5000U
        );

    g_throttleRmtSymbols =
        RMT_SYMBOLS_PER_CHANNEL_BLOCK;

    const bool throttleRmtReadOk =
        rmtReadAsync(
            PIN_RC_THROTTLE,
            g_throttleRmtData,
            &g_throttleRmtSymbols
        );

    g_throttleRmtReceptionActive =
        throttleRmtInitOk &&
        throttleRmtMinOk &&
        throttleRmtMaxOk &&
        throttleRmtReadOk;
          
   
}

// ============================================================
// Read captured inputs
// ============================================================

void halOperatorInputRead(HalOperatorInput& input)
{
    uint32_t modePulseUs;
    uint32_t modeLastUpdateUs;

    uint32_t throttlePulseUs;
    uint32_t throttleLastUpdateUs;

   
   if (g_modeRmtReceptionActive &&
    rmtReceiveCompleted(PIN_RC_MODE))
    {
        for (size_t i = 0U; i < g_modeRmtSymbols; ++i)
        {
            const rmt_data_t& symbol =
                g_modeRmtData[i];

            if ((symbol.level0 == 1U) &&
                (symbol.duration0 >= RC_MIN_VALID_PULSE_US) &&
                (symbol.duration0 <= RC_MAX_VALID_PULSE_US))
            {
                g_modePulseUs = symbol.duration0;
                g_modeLastUpdateUs = micros();
            }

            if ((symbol.level1 == 1U) &&
                (symbol.duration1 >= RC_MIN_VALID_PULSE_US) &&
                (symbol.duration1 <= RC_MAX_VALID_PULSE_US))
            {
                g_modePulseUs = symbol.duration1;
                g_modeLastUpdateUs = micros();
            }
        }

        // Start next asynchronous capture.
        g_modeRmtSymbols =
            RMT_SYMBOLS_PER_CHANNEL_BLOCK;

        g_modeRmtReceptionActive =
            rmtReadAsync(
                PIN_RC_MODE,
                g_modeRmtData,
                &g_modeRmtSymbols
            );
    }
   
    if (g_throttleRmtReceptionActive &&
        rmtReceiveCompleted(PIN_RC_THROTTLE))
    {
        for (size_t i = 0U; i < g_throttleRmtSymbols; ++i)
        {
            const rmt_data_t& symbol =
                g_throttleRmtData[i];

            if ((symbol.level0 == 1U) &&
                (symbol.duration0 >= RC_MIN_VALID_PULSE_US) &&
                (symbol.duration0 <= RC_MAX_VALID_PULSE_US))
            {
                g_throttlePulseUs = symbol.duration0;
                g_throttleLastUpdateUs = micros();
            }

            if ((symbol.level1 == 1U) &&
                (symbol.duration1 >= RC_MIN_VALID_PULSE_US) &&
                (symbol.duration1 <= RC_MAX_VALID_PULSE_US))
            {
                g_throttlePulseUs = symbol.duration1;
                g_throttleLastUpdateUs = micros();
            }
        }

        g_throttleRmtSymbols =
            RMT_SYMBOLS_PER_CHANNEL_BLOCK;

        g_throttleRmtReceptionActive =
            rmtReadAsync(
                PIN_RC_THROTTLE,
                g_throttleRmtData,
                &g_throttleRmtSymbols
            );
    }

  // Copy latest captured RMT values.
   
    modePulseUs = g_modePulseUs;
    modeLastUpdateUs = g_modeLastUpdateUs;

    throttlePulseUs = g_throttlePulseUs;
    throttleLastUpdateUs = g_throttleLastUpdateUs;

    
    const uint32_t nowUs = micros();

    

    // --------------------------------------------------------
    // MODE
    // --------------------------------------------------------

    input.modePulseUs = modePulseUs;

    input.modeValid =
    (modePulseUs >= RC_MIN_VALID_PULSE_US) &&
    (modePulseUs <= RC_MAX_VALID_PULSE_US) &&
    (modeLastUpdateUs != 0U) &&
    ((uint32_t)(nowUs - modeLastUpdateUs)
        < HAL_OPERATOR_INPUT_SIGNAL_TIMEOUT_US);

    // --------------------------------------------------------
    // THROTTLE
    // --------------------------------------------------------

    input.throttlePulseUs = throttlePulseUs;

    input.throttleValid =
        (throttlePulseUs >= RC_MIN_VALID_PULSE_US) &&
        (throttlePulseUs <= RC_MAX_VALID_PULSE_US) &&
        (throttleLastUpdateUs != 0U) &&
        ((uint32_t)(nowUs - throttleLastUpdateUs)
            < HAL_OPERATOR_INPUT_SIGNAL_TIMEOUT_US);
}

void halOperatorInputGetLatest(
    HalOperatorInput& input)
{
    const uint32_t nowUs = micros();

    input.modePulseUs = g_modePulseUs;

    input.modeValid =
        (g_modePulseUs >= RC_MIN_VALID_PULSE_US) &&
        (g_modePulseUs <= RC_MAX_VALID_PULSE_US) &&
        (g_modeLastUpdateUs != 0U) &&
        ((uint32_t)(nowUs - g_modeLastUpdateUs) <
            HAL_OPERATOR_INPUT_SIGNAL_TIMEOUT_US);

    input.throttlePulseUs = g_throttlePulseUs;

    input.throttleValid =
        (g_throttlePulseUs >= RC_MIN_VALID_PULSE_US) &&
        (g_throttlePulseUs <= RC_MAX_VALID_PULSE_US) &&
        (g_throttleLastUpdateUs != 0U) &&
        ((uint32_t)(nowUs - g_throttleLastUpdateUs) <
            HAL_OPERATOR_INPUT_SIGNAL_TIMEOUT_US);
}