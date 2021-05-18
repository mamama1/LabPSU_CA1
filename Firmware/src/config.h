#ifndef config_h
	#define config_h

	#define EEPROM_ADDR_INIT_INDICATOR		0
	#define EEPROM_ADDR_CALIBRATION			9

	#define LCD_COLS						20
	#define LCD_ROWS						4

	#define DISPLAY_UPDATE_INVERVAL			500
	#define BUTTON_REPEAT_MENU_INTERVAL		500
	#define BUTTON_REPEAT_VALUE_INTERVAL	100

	// #define DAC_FAN_UPPER_LIMIT				255
	// #define DAC_FAN_LOWER_LIMIT				0

	#define DAC_V_LOWER_LIMIT				0
	#define DAC_V_UPPER_LIMIT				2500 // (Vout (max) in mV - VOUT_OFFSET_MV) / VOUT_GAIN

	#define VOLTAGE_UPPER_LIMIT				25000	// mV
	#define VOLTAGE_LOWER_LIMIT				0		// mV
	#define CURRENT_UPPER_LIMIT				1000	// mA
	#define CURRENT_LOWER_LIMIT				0		// mA

	#define DAC_I_LOWER_LIMIT				0
	#define DAC_I_UPPER_LIMIT				4000	//  500 bei 100mOhm Shunt, 4000 bei 800mOhm Shunt   Iout (max) in mA * 2 --- MAX4080T outputs e.g. 100mV for 50mA load so 1000mV upper limit will be 500mA Iout (max)
													// MAX4080F - Gain: 5 - 1A @ 0.1R Shunt = 0.1V * 5 (Gain) = 0.5V ---	1000mA Imax = DAC_I_UPPER_LIMIT 500
													//																		2000mA Imax = DAC_I_UPPER_LIMIT 1000
													//																		Formula:	DAC_I_UPPER_LIMIT = Imax / 2
													//																					Imax = DAC_I_UPPER_LIMIT * 2

	#define DAC_V_STARTUP_VALUE				DAC_V_LOWER_LIMIT
	#define DAC_I_STARTUP_VALUE				DAC_I_UPPER_LIMIT

	#define DAC_VREF						4096UL
	#define GAIN_V_DAC						2U	// 2x DAC internal Gain
	#define GAIN_V_CONTROL_LOOP				5U	// 5x Control loop Gain
	
	#define GAIN_I_DAC						1
	
	#define GAIN_I_CONTROL_LOOP				20U	// 5x Control loop Gain
	// #define GAIN_I_CONTROL_LOOP				20U	// 5x Control loop Gain
	// #define ADC_CURRENT_SHUNT_RESISTANCE	0.8f	// Ohm
	#define ADC_CURRENT_SHUNT_RESISTANCE	0.2f	// Ohm
	
	#define DAC_V_STEPSIZE					10

	#define DAC_I_STEPSIZE					4 // 1 bei 100mOhm Shunt, 4 bei 800mOhm Shunt
	#define DAC_MIN_SET_DELAY				100

	#define DAC_V_STEPS						((DAC_V_UPPER_LIMIT - DAC_V_LOWER_LIMIT) / DAC_V_STEPSIZE)
	// #define DAC_I_STEPS						((DAC_I_UPPER_LIMIT - DAC_I_LOWER_LIMIT) / DAC_I_STEPSIZE) + 1

	#define DAC_BITS						4096UL // 12 Bit
	// #define DAC_OFFSET						1		// MCP4922 step 0 of 4095 means first step (VREF/4096), not zero. So 0 would be 0,5mV for 2048mV VREF.

	#define ADC_VREF						4096UL
	#define ADC_BITS						1024UL

	// #define ADC_INPUT_VOLTAGE_DIVIDER		6U
	#define ADC_INPUT_VOLTAGE_DIVIDER		6.25f

	#define ADC_INPUT_CURRENT_DIVIDER		1U
	// #define ADC_V_READ_MULTIPLICATOR		(ADC_VREF * ADC_INPUT_VOLTAGE_DIVIDER / ADC_BITS)
	// #define ADC_I_READ_MULTIPLICATOR		(ADC_CURRENT_SHUNT_RESISTANCE * GAIN_I_CONTROL_LOOP * ADC_VREF * ADC_INPUT_CURRENT_DIVIDER / ADC_BITS)
	#define ADC_POLL_INTERVAL				100
	
	#define ADC_AVERAGING					4

	// #define ADC_TEENSY_VREF					2048
	// #define ADC_TEENSY_BITS					4096
	// #define ADC_TEENSY_POLL_INTERVAL		100
	// #define ADC_TEENSY_AVERAGING			32

	// #define ADC_CURRENT_GAIN				50		// Current shunt amplifier gain factor

	#define DEBOUNCE_INTERVAL				10

	// TM Temperature Monitor
	// #define TM_DIVIDER_VOLTAGE				15000
	// #define TM_DIVIDER_R1					681000
	// #define TM_DIVIDER_NTC_NOMINAL			100000
	// #define TM_DIVIDER_NTC_COEFFICIENT		4190

	// #define BUZZER_ENABLED
	// define DISPLAY_UPDATE_ON_ADC_UPDATE

#endif