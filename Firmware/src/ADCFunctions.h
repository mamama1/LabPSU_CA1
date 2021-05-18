#pragma once

#include <Arduino.h>

extern uint32_t lastADCupdateMillis;

extern uint16_t adcVoltageReadAverage;
extern uint16_t adcCurrentReadAverage;
extern uint16_t adcOvervoltageAverage;

extern uint16_t adcVoltageRead[ADC_AVERAGING];
extern uint16_t adcCurrentRead[ADC_AVERAGING];
extern uint16_t adcOvervoltage[ADC_AVERAGING];

extern uint8_t adcSampleIndex;

void processADC()
{
	if (millis() - lastADCupdateMillis > ADC_POLL_INTERVAL)
	{
		adcVoltageRead[adcSampleIndex] = analogRead(ADC_VOLTAGE_READ_CH);
		adcCurrentRead[adcSampleIndex] = analogRead(ADC_CURRENT_READ_CH);
		adcOvervoltage[adcSampleIndex] = analogRead(ADC_OVRCRNT_DET_CH);
		
		adcSampleIndex++;

		if (adcSampleIndex >= ADC_AVERAGING)
		{
			adcSampleIndex = 0;
		}

		adcVoltageReadAverage = 0;
		adcCurrentReadAverage = 0;
		adcOvervoltageAverage = 0;

		for (uint8_t n = 0; n < ADC_AVERAGING; n++)
		{
			adcVoltageReadAverage += adcVoltageRead[n];
			adcCurrentReadAverage += adcCurrentRead[n];
			adcOvervoltageAverage += adcOvervoltage[n];
		}

		adcVoltageReadAverage = adcVoltageReadAverage / ADC_AVERAGING;
		adcCurrentReadAverage = adcCurrentReadAverage / ADC_AVERAGING;
		adcOvervoltageAverage = adcOvervoltageAverage / ADC_AVERAGING;

		lastADCupdateMillis = millis();
	}
}