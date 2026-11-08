#include "hal_display.h"
#include "hal_board_config.h"

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

static constexpr uint8_t OLED_WIDTH = 128U;
static constexpr uint8_t OLED_HEIGHT = 64U;
static constexpr uint8_t OLED_ADDRESS = 0x3CU;

static Adafruit_SSD1306 g_display(
    OLED_WIDTH,
    OLED_HEIGHT,
    &Wire,
    -1
);

static bool g_displayAvailable = false;

bool halDisplayInit()
{
    Wire.begin(
        static_cast<int>(PIN_I2C_SDA),
        static_cast<int>(PIN_I2C_SCL)
    );

    if (!g_display.begin(
            SSD1306_SWITCHCAPVCC,
            OLED_ADDRESS))
    {
        g_displayAvailable = false;
        return false;
    }

    g_display.clearDisplay();
    g_display.setTextColor(SSD1306_WHITE);
    g_display.setTextSize(1U);
    g_display.display();

    g_displayAvailable = true;
    return true;
}

void halDisplayClear()
{
    if (!g_displayAvailable)
    {
        return;
    }

    g_display.clearDisplay();
}

void halDisplayPrintLine(
    uint8_t line,
    const char* text
)
{
    if (!g_displayAvailable)
    {
        return;
    }

    const int16_t y =
        static_cast<int16_t>(line) * 16;

    g_display.setCursor(0, y);
    g_display.print(text);
}

void halDisplayUpdate()
{
    if (!g_displayAvailable)
    {
        return;
    }

    g_display.display();
}

bool halDisplayIsAvailable()
{
    return g_displayAvailable;
}