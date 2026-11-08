// hal_pwm.h
#pragma once

void halPwmInit();

void halStarterEscWrite(float commandPercent);
void halFuelPumpEscWrite(float commandPercent);