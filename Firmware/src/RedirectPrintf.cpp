#include <Arduino.h>
#include "RedirectPrintf.h"

#if defined(REDIRECT_PRINTF) && (REDIRECT_PRINTF > 0)

	#if (REDIRECT_PRINTF == REDIRECT_PRINTF_FEATURELEVEL_MILLIS)
		uint32_t LastLOGMillis;
		uint32_t CurrentLOGMillis;
	#endif

	HardwareSerial *DebugSerial;

	#if defined(ARDUINO_ARCH_AVR)

		int SerialPutChar(char c, FILE *)
		{
			DebugSerial->write(c);
			return c;
		}

	#endif

	bool PrintfBegin(HardwareSerial *serial)
	{
		if (serial == NULL)
			return false;

		DebugSerial = serial;
		DebugSerial->begin(DEBUG_BAUD_RATE);
		
		#if defined (ARDUINO_ARCH_AVR)
			fdevopen(&SerialPutChar, 0);
		#elif defined (__ARDUINO_X86__)
			stdout = freopen("/dev/ttyGS0", "w", stdout);
			delay(500);
		#endif

		printf_P(PSTR("\r\n\r\n\r\n / \\  %s - %s\r\n( C ) Running Firmware Ver.: %u.%u.%u\r\n \\ /  From %s - %s\r\n"), PRODUCT_FAMILY, PRODUCT_NAME, FW_MAJOR, FW_MINOR, FW_PATCH, __DATE__, __TIME__);
		delay(1);

		return true;
	}

#else

	bool PrintfBegin(HardwareSerial *serial)
	{
		delay(1);
		return false;
	}

#endif

// void LOG2(char const *file, char const *line, char const *fmt, va_list args)
// {
// 	//va_list args;
// 	va_start(args, fmt);

// 	CurrentLOGMillis = millis();
// 	printf_P(PSTR("%07lu (%+06lu) | %s:%03u | "), CurrentLOGMillis, CurrentLOGMillis - LastLOGMillis, file, line);
// 	LastLOGMillis = CurrentLOGMillis;

// 	printf_P(fmt, args);
// 	//vprintf(fmt, args);
// 	va_end(args);
// }