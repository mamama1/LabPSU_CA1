#pragma once

#include <Arduino.h>
#include <RedirectPrintf.h>
#include <dnxMCP4922.h>
#include <config.h>
#include <pins.h>

dnxMCP4922 DAC_SPI(DAC_CS);

uint16_t lastRawDACval = 0;

void SetDACVoltage(uint16_t voltage);
void SetDACCurrent(uint16_t current);
void SetDACVoltageRaw(uint16_t value);
void SetDACCurrentRaw(uint16_t value);

void initDAC(uint16_t voltage, uint16_t current)
{
	SetDACVoltage(voltage);
	SetDACCurrent(current);
}

void SetDACVoltage(uint16_t voltage)
{
	uint16_t dacValue = voltage * DAC_BITS / (DAC_VREF * GAIN_V_DAC * GAIN_V_CONTROL_LOOP);
	LOG("SetDACVoltage: %u mV\r\n", voltage);
	SetDACVoltageRaw(dacValue);
}

void SetDACVoltageRaw(uint16_t value)
{
	LOG("SetDACCurrent dacValue: %u\r\n", value);
	lastRawDACval = value;
	DAC_SPI.SetB(value);
}

void SetDACCurrent(uint16_t current)
{
	uint16_t dacValue = current * ADC_CURRENT_SHUNT_RESISTANCE * DAC_BITS * GAIN_I_CONTROL_LOOP / (DAC_VREF * GAIN_I_DAC);
	LOG("SetDACCurrent: %u mA\r\n", current);
	SetDACCurrentRaw(dacValue);
}

void SetDACCurrentRaw(uint16_t value)
{
	LOG("SetDACCurrent dacValue: %u\r\n", value);
	DAC_SPI.SetA(value);
}
