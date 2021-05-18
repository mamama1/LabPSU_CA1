/// @file RedirectPrintf.h
/// @author Alexej Goujine (dev@devnetx.at)
/// 
/// @version 1.4
/// @date 2020-04-27
/// 
/// @copyright Copyright (c) 2020 devnetX

#pragma once

#include <Arduino.h>
#include <stdarg.h>

#define REDIRECT_PRINTF_FEATURELEVEL_OFF		0
#define REDIRECT_PRINTF_FEATURELEVEL_MIN		1
#define REDIRECT_PRINTF_FEATURELEVEL_POSITION	2
#define REDIRECT_PRINTF_FEATURELEVEL_MILLIS 	3

// #define REDIRECT_PRINTF 0
// #define REDIRECT_PRINTF 1
// #define REDIRECT_PRINTF 2
// #define REDIRECT_PRINTF 3

// ; build_flags = -D REDIRECT_PRINTF=0
// ; build_flags = -D REDIRECT_PRINTF=1
// ; build_flags = -D REDIRECT_PRINTF=2
// ; build_flags = -D REDIRECT_PRINTF=3

#if defined(REDIRECT_PRINTF) && (REDIRECT_PRINTF > 0)

	// #define DEBUG_BAUD_RATE				57600UL
	#define DEBUG_BAUD_RATE			115200UL

	#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
	#define __FILENAME_NOCHECK__ (strrchr(__FILE__, '\\') + 1)

	extern HardwareSerial *DebugSerial;
	bool PrintfBegin(HardwareSerial *serial);
	
	#if (REDIRECT_PRINTF == REDIRECT_PRINTF_FEATURELEVEL_MILLIS)

		extern uint32_t LastLOGMillis;
		extern uint32_t CurrentLOGMillis;

		void LOG2(char const *file, char const *line, char const *fmt, va_list argp);

		#define LOG3(fmt, ...) \
		{ \
			LOG2(__FILE__, __LINE__, PSTR(fmt), ##__VA_ARGS__); \
		}

		#define LOG(fmt, ...) \
		{ \
			CurrentLOGMillis = millis(); \
			printf_P(PSTR("%07lu (%+06lu) | %s:%u | " fmt), \
						  CurrentLOGMillis, CurrentLOGMillis - LastLOGMillis, \
						  __FILE__, __LINE__, ##__VA_ARGS__); \
			LastLOGMillis = CurrentLOGMillis; \
		}

	#elif (REDIRECT_PRINTF == REDIRECT_PRINTF_FEATURELEVEL_POSITION)

		#define LOG(fmt, ...) \
		{ \
			printf_P(PSTR("%s:%u | " fmt), __FILE__, __LINE__, ##__VA_ARGS__); \
		}

	#elif (REDIRECT_PRINTF == REDIRECT_PRINTF_FEATURELEVEL_MIN)
		
		#define LOG(fmt, ...) \
		{ \
			printf_P(PSTR(fmt), ##__VA_ARGS__); \
		}

	#endif

	#define PRINT(fmt, ...) { printf_P(PSTR(fmt), ##__VA_ARGS__); }

#else

	bool PrintfBegin(HardwareSerial *serial);
	
	#define printf(fmt, ...) {}
	#define printf_P(fmt, ...) {}
	#define LOG(fmt, ...) {}
	#define PRINT(fmt, ...) {}

#endif

