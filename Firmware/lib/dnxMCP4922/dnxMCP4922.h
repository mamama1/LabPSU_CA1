#ifndef dnxMCP4922_h
#define dnxMCP4922_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#include "SPI.h"
#else
#include "WProgram.h"
#endif


class dnxMCP4922
{
	public:
		dnxMCP4922(uint8_t CS);
		void Set(uint16_t A, uint16_t B);
		void SetA(uint16_t val);
		void SetB(uint16_t val);

	private:
		uint8_t _CS;
		SPISettings _spiSettings;

		void sendIntValueSPI(uint16_t val, uint8_t channel);
};

#endif
