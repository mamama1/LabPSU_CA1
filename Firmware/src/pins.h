#ifndef pins_h
	#define pins_h

	// ##### Pin definitions

	#define DAC_CS							A0 //18
	
	#define BUTTON_DOWN_PIN					22
	#define BUTTON_UP_PIN					19
	#define BUTTON_ENTER_PIN				21
	#define BUTTON_OUTPUT_PIN				23
	#define LED_STATUS_PIN					12
	#define LED_OUTPUT_PIN					11
	#define LCD_RS_PIN						14
	#define LCD_EN_PIN						13
	#define LCD_DB0_PIN						2
	#define LCD_DB1_PIN						3
	#define LCD_DB2_PIN						0
	#define LCD_DB3_PIN						1
	#define LCD_DB4_PIN						15
	#define LCD_DB5_PIN						16
	#define LCD_DB6_PIN						17
	#define LCD_DB7_PIN						18
	#define REL_OUT_PIN						20

	#define ADC_CURRENT_READ_CH				A4 //A1	// onboard current sensing		### A1
	#define ADC_OVRCRNT_DET_CH				A5 //A2	// overcurrent detection		### A2
	#define ADC_VOLTAGE_READ_CH				A6
#endif