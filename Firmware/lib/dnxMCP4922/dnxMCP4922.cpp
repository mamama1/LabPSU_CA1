#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "dnxMCP4922.h"
#include <SPI.h>


dnxMCP4922::dnxMCP4922(uint8_t CS)
{
	_CS = CS;

	pinMode(_CS,OUTPUT);
	digitalWrite(_CS,HIGH);
	SPI.begin();
	//SPI.setClockDivider(SPI_CLOCK_DIV8);
	_spiSettings = SPISettings(2000000, MSBFIRST, SPI_MODE0);
}

//************************************************************************
void dnxMCP4922::Set(uint16_t A, uint16_t B) {
	sendIntValueSPI(A, 0);
	sendIntValueSPI(B, 1);
}

void dnxMCP4922::SetA(uint16_t val)
{
	sendIntValueSPI(val, 0);
}

void dnxMCP4922::SetB(uint16_t val)
{
	sendIntValueSPI(val, 1);
}
//************************************************************************ 
/*
Bitmasking for setting options in dac:

The four MSB in the Mask 0b0111000000000000 and 0b1111000000000000 is for
setting different options of the DAC setup.

0bX111000000000000 where X is What DAC channel the SPI is writing to.
bit15			   X=0 is writing to channel A.
				   X=1 is writing to channel B.

0b0X11000000000000 where X is Buffered or UnBuffered mode. Buffered uses LDAC 
bit14			   pin to simuttaneous update both channels.
				   UnBuffered I guess is writing outputs directly to DAC 
				   outputs and ignoring LDAC pin.
				   X=0 is UnBuffered.
				   X=1 is Buffered.
				   
0b01x1000000000000 where X is GAIN selector.  
bit13			   X=0 is 2X GAIN.
				   X=1 is 1X GAIN.
				   
0b011X000000000000 where X SHUTDOWN.
bit12			   X=0 OUTPUT is DISABLED on selected channel.
				   X=1 OUTPUT is ENABLED on selected channel.
			
0b0111XXXXXXXXXXXX where X is the 12 bits to be written to the active channel.
bit 11 down to bit 0			

 */
//************************************************************************

void dnxMCP4922::sendIntValueSPI(uint16_t val, uint8_t channel) {

	uint16_t writeVal;
	if (channel == 0)
	{
		writeVal = val | 0b0111000000000000;
	}
	else
	{
		writeVal = val | 0b1101000000000000;	
	}
	SPI.beginTransaction(_spiSettings);
	digitalWrite(_CS, LOW);
	SPI.transfer(highByte(writeVal));
	SPI.transfer(lowByte(writeVal));
	digitalWrite(_CS, HIGH);
	SPI.endTransaction();

	__asm__("nop\n\t");
	//delay(1);
}
 
 

 