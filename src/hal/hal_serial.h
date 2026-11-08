#pragma once

#include <stdint.h>

void halSerialInit(uint32_t baudRate);

void halSerialPrint(const char* text);
void halSerialPrintln(const char* text);

void halSerialPrintUint32(uint32_t value);
void halSerialPrintFloat(float value, uint8_t decimals);