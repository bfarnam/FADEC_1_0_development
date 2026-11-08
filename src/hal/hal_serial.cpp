#include "hal_serial.h"

#include <Arduino.h>

void halSerialInit(uint32_t baudRate)
{
    Serial.begin(baudRate);
    delay(200);
}

void halSerialPrint(const char* text)
{
    Serial.print(text);
}

void halSerialPrintln(const char* text)
{
    Serial.println(text);
}

void halSerialPrintUint32(uint32_t value)
{
    Serial.print(value);
}
void halSerialPrintFloat(float value, uint8_t decimals)
{
    Serial.print(value, decimals);
}