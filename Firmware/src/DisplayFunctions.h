#pragma once

#include <Arduino.h>

byte symCL[8] = {
	B01111,
	B11001,
	B10111,
	B10111,
	B10111,
	B11001,
	B01111,
	B00000
};

byte symCR[8] = {
	B11110,
	B11001,
	B10111,
	B10111,
	B10111,
	B11001,
	B11110,
	B00000
};

byte symVR[8] = {
	B11110,
	B10101,
	B10101,
	B10101,
	B10101,
	B11011,
	B11110,
	B00000
};

byte symSettings[8] = {
  B00000,
  B00010,
  B11101,
  B00010,
  B01000,
  B10111,
  B01000,
  B00000
};

byte symFull[8] = {
	B11111,
	B11111,
	B11111,
	B11111,
	B11111,
	B11111,
	B11111,
	B11111
};

typedef enum class OpMode : uint8_t
{
	SELF_TEST_START =				0U,
	SELF_TEST_PHASE1 = 				1U,
	SELF_TEST_PHASE2 = 				2U,
	SELF_TEST_PHASE3 = 				3U,
	SELF_TEST_PHASE4 = 				4U,
	SELF_TEST_PHASE5 = 				5U,
	SELF_TEST_COMPLETED =			6U,
	CALIBRATION_START =				7U,
	CALIBRATING_WAIT_SETTLE =		8U,
	CALIBRATING	=					9U,
	CALIBRATION_FINISHED_WAITING = 	10U,
	CALIBRATION_FINISHED_EXIT = 	11U,
	REGULAR = 						12U,
	FAILED =						13U
} OpMode_t;

typedef enum class FailureReason : uint8_t
{
	SELF_TEST_PHASE1_CURRENT =		0U,
	SELF_TEST_PHASE1_VOLTAGE =		1U,
	SELF_TEST_PHASE2_CURRENT =		2U,
	SELF_TEST_PHASE2_VOLTAGE =		3U,
	SELF_TEST_PHASE3_CURRENT =		4U,
	SELF_TEST_PHASE3_VOLTAGE = 		5U,
	SELF_TEST_PHASE4_CURRENT =		6U,
	SELF_TEST_PHASE4_VOLTAGE = 		7U,
	SELF_TEST_PHASE5_CURRENT =		8U,
	SELF_TEST_PHASE5_VOLTAGE = 		9U
} FailureReason_t;

FailureReason_t errors[10];
int8_t lastErrorIndex = -1;

typedef enum class MessageVAlignment : uint8_t
{
	TOP = 		0U,
	MIDDLE =	1U,
	BOTTOM =	2U
} MessageVAlignment_t;

OpMode_t currentOpMode;

uint32_t lastDisplayUpdateMillis = 0;

char messageLines[LCD_ROWS][LCD_COLS + 1] = {0, };

extern HomeMenuItem_t *SelectedMenu;
extern LiquidCrystal LCD;

extern uint16_t adcVoltageReadAverage;
extern uint16_t adcCurrentReadAverage;
extern uint16_t adcOvervoltageAverage;

extern uint16_t adcVoltageReadAverage_prev;
extern uint16_t adcCurrentReadAverage_prev;
extern uint16_t adcOvervoltageAverage_prev;

// extern bool adcVoltageReadAverageChanged;
// extern bool adcCurrentReadAverageChanged;
// extern bool adcOvervoltageAverageChanged;

extern uint16_t displayVoltageRead;
extern uint16_t displayCurrentRead;

extern uint16_t lastRawDACval;
extern uint16_t calibrationValues[DAC_V_STEPS];


void UpdateDisplay();

void initDisplay()
{
	LCD.begin(LCD_COLS, LCD_ROWS);

	LCD.createChar(0, symCL);
	LCD.createChar(1, symCR);
	LCD.createChar(2, symVR);
	LCD.createChar(3, symSettings);
	LCD.createChar(4, symFull);
}

void drawHomeScaffold()
{
	LCD.setCursor(0, 0);
	LCD.print("     SET|   READ|   "); LCD.setCursor(0, 1);
	LCD.print("> 0.000V| 0.000V|   "); LCD.setCursor(0, 2);
	LCD.print("  0.000A| 0.000A|   "); LCD.setCursor(0, 3);
	LCD.print("  0.000W| 0.000W|   ");

	LCD.setCursor(2, 0);
	LCD.write(byte(3));
}

void displaySetValues()
{
	HomeMenuItem_t *LastSelectedMenu = SelectedMenu;

	SelectedMenu = &VoltageMenu_E;
	UpdateDisplay();
	SelectedMenu = &VoltageMenu_z;
	UpdateDisplay();
	SelectedMenu = &CurrentMenu_E;
	UpdateDisplay();
	SelectedMenu = &CurrentMenu_z;
	UpdateDisplay();
	SelectedMenu = &CurrentMenu_h;
	UpdateDisplay();
	SelectedMenu = &CurrentMenu_t;
	UpdateDisplay();

	SelectedMenu = LastSelectedMenu;
	UpdateDisplay();
}

void displayConfigMenu()
{
	LCD.clear();
	LCD.setCursor(2, 0);
	LCD.print(ConfigMenu_Calibrate.Caption);
	LCD.setCursor(2, 1);
	LCD.print(ConfigMenu_Exit.Caption);
}

void displayMessage(const char *message, MessageVAlignment_t alignment)
{
	uint8_t len = strlen(message);
	uint8_t line = 0;
	uint8_t idx = 0;

	for (uint8_t n = 0; n < len; n++)
	{
		if (message[n] == '\r')
		{
			messageLines[line][idx] = '\0';
			if (line < LCD_ROWS)
			{
				line++;
			}
			idx = 0;
		}
		else if (message[n] == '\n')
		{
			// nop
		}
		else
		{
			messageLines[line][idx] = message[n];
			if (idx < LCD_COLS)
			{
				idx++;
			}				
		}
	}
	messageLines[line][idx] = '\0';

	LOG("\r\nlen: %u\r\nlines: %u\r\nLine 0: >%s<\r\nLine 1: >%s<\r\nLine 2: >%s<\r\nLine 3: >%s<\r\n", len, line + 1, messageLines[0], messageLines[1], messageLines[2], messageLines[3]);

	uint8_t msgY = 0;

	if (alignment == MessageVAlignment::TOP)
	{
		msgY = 0;
	}
	else if (alignment == MessageVAlignment::MIDDLE)
	{
		msgY =  (LCD_ROWS - (line + 1)) / 2;
	}
	else if (alignment == MessageVAlignment::BOTTOM)
	{
		msgY = LCD_ROWS - (line + 1);
	}

	uint8_t xpos = 0;

	for (uint8_t n = 0; n <= line; n++)
	{
		xpos  = (LCD_COLS - strlen(messageLines[n])) / 2;
		LCD.setCursor(xpos, msgY + n);
		LCD.print(messageLines[n]);
	}
}

void displayValue(uint16_t val)
{
	if (val < 10000)
		LCD.print(' ');

	LCD.print((int)(val / 1000));
	LCD.print('.');

	val %= 1000;

	if (val < 10)
		LCD.print("00");
	else if (val < 100)
		LCD.print("0");
	
	LCD.print(val);

	// LOG("XXX = %u\r\n", val);	
}

void UpdateDisplay()
{
	if (SelectedMenu->type == MenuType::MESSAGE)
		return;

	if (SelectedMenu->type == MenuType::MAIN_SCREEN_VAL || SelectedMenu->type == MenuType::CONFIG_MENU_VAL)
	{
		LCD.cursor();
		LCD.blink();
	}
	else
	{
		LCD.noCursor();
		LCD.noBlink();

		for (uint8_t n = 0; n < 4; n++)
		{
			LCD.setCursor(0, n);
			
			if (n == SelectedMenu->CursorY)
				LCD.print('>');
			else
				LCD.print(' ');
		}
	}

	if (SelectedMenu->type == MenuType::CONFIG_MENU)
	{
		return;
	}
	
	if (SelectedMenu->type == MenuType::MAIN_SCREEN_VAL)
	{
		if (SelectedMenu == &VoltageMenu_E)
		{
			LCD.setCursor(SelectedMenu->CursorX - 1, SelectedMenu->CursorY);
			
			if (SelectedMenu->Digit < 10)
			{
				LCD.print(' ');
				LCD.print(SelectedMenu->Digit);
			}
			else
			{
				LCD.print(SelectedMenu->Digit);
			}			
		}
		else
		{
			LCD.setCursor(SelectedMenu->CursorX, SelectedMenu->CursorY);
			LCD.print(SelectedMenu->Digit);
		}

		LCD.setCursor(SelectedMenu->CursorX, SelectedMenu->CursorY);
	}
}

void UpdateDisplayRead(bool force = false)
{
	if (currentOpMode != OpMode::REGULAR)
	{
		return;
	}

	if (SelectedMenu->type == MenuType::CONFIG_MENU && !force)
	{
		return;
	}

	if ((millis() - lastDisplayUpdateMillis > DISPLAY_UPDATE_INVERVAL) || force)
	{
		if (adcVoltageReadAverage != adcVoltageReadAverage_prev || force)
		{
			// uint16_t displayVoltageRead = adcVoltageReadAverage * ADC_V_READ_MULTIPLICATOR;
			displayVoltageRead = adcVoltageReadAverage * ADC_VREF * ADC_INPUT_VOLTAGE_DIVIDER / ADC_BITS;
			LOG("ADC V Read: %u\r\n", displayVoltageRead);

			LCD.setCursor(9, 1);
			displayValue(displayVoltageRead);

			LCD.setCursor(9, 3);
			displayValue((uint16_t)((float)displayVoltageRead * (float)displayCurrentRead / 1000));

			adcVoltageReadAverage_prev = adcVoltageReadAverage;
		}

		if (adcCurrentReadAverage != adcCurrentReadAverage_prev || force)
		{
			displayCurrentRead = adcCurrentReadAverage * ADC_VREF * ADC_INPUT_CURRENT_DIVIDER / (ADC_CURRENT_SHUNT_RESISTANCE * GAIN_I_CONTROL_LOOP * ADC_BITS);

			if (displayCurrentRead >= calibrationValues[lastRawDACval / DAC_V_STEPSIZE])
				displayCurrentRead -= calibrationValues[lastRawDACval / DAC_V_STEPSIZE];
			else
				displayCurrentRead = 0;

			LOG("ADC I Read: %u\r\n", displayCurrentRead);

			LCD.setCursor(9, 2);
			displayValue(displayCurrentRead);

			LCD.setCursor(9, 3);
			displayValue((uint16_t)((float)displayVoltageRead * (float)displayCurrentRead / 1000));

			adcCurrentReadAverage_prev = adcCurrentReadAverage;
		}

		if (adcOvervoltageAverage != adcOvervoltageAverage_prev || force)
		{
			LCD.setCursor (18, 1);
			LCD.print("  ");
			LCD.setCursor (18, 2);
			LCD.print("  ");

			if (adcOvervoltageAverage < 1023)
			{
				// CC mode
				LCD.setCursor(18, 2);
				LCD.write(byte(0));		// symCL
				LCD.write(byte(1));		// symCR
				digitalWrite(LED_STATUS_PIN, HIGH);
			}
			else
			{
				// CV mode
				LCD.setCursor(18, 1);
				LCD.write(byte(0));		// symCL
				LCD.write(byte(2));		// symVR
				digitalWrite(LED_STATUS_PIN, LOW);
			}

			adcOvervoltageAverage_prev = adcOvervoltageAverage;
		}
	}
}

void updateDisplaySetWattage()
{
	LCD.setCursor(1, 3);
	displayValue((uint16_t)((float)(VoltageMenu_E.Digit * 1000 + VoltageMenu_z.Digit * 100) * (float)(CurrentMenu_E.Digit * 1000 + CurrentMenu_z.Digit * 100 + CurrentMenu_h.Digit * 10 + CurrentMenu_t.Digit * 1) / 1000));
}

void processCursor()
{
	if (currentOpMode != OpMode::REGULAR)
		return;

	if (SelectedMenu->type == MenuType::CONFIG_MENU_VAL || SelectedMenu->type == MenuType::MAIN_SCREEN_VAL)
		LCD.setCursor(SelectedMenu->CursorX, SelectedMenu->CursorY);
}

void displayMainScreen()
{
	drawHomeScaffold();
	displaySetValues();					// refresh set values to display
	UpdateDisplayRead(true);			// update ADC read values
	updateDisplaySetWattage();
}