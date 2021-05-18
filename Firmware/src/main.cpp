#include <Arduino.h>
#include <EEPROM.h>
#include <pins.h>
#include <config.h>

#include <AVRFunctional.h>
#include <RedirectPrintf.h>
#include <ButtonDebouncer.h>

#include <LiquidCrystal.h>
#include <dnxMCP4922.h>

#include <DACFunctions.h>
#include <HomeMenu.h>
#include <DisplayFunctions.h>
#include <ButtonFunctions.h>
#include <ADCFunctions.h>

LiquidCrystal LCD(LCD_RS_PIN, LCD_EN_PIN, LCD_DB0_PIN, LCD_DB1_PIN, LCD_DB2_PIN, LCD_DB3_PIN,LCD_DB4_PIN, LCD_DB5_PIN, LCD_DB6_PIN, LCD_DB7_PIN);

uint16_t currentCalibrationStep = 0;
uint32_t calibrationADCwaitMillis = 0;
uint16_t calibrationValues[DAC_V_STEPS] = { 0, };
uint8_t calibrationPercentage = 0;
uint8_t calibrationPercentage_prev = 0;

uint32_t selfTestWaitMillis = 0;

ButtonDebouncer ButtonUp;
ButtonDebouncer ButtonEnter;
ButtonDebouncer ButtonDown;
ButtonDebouncer ButtonOutput;

uint32_t lastADCupdateMillis = 0;

uint16_t adcVoltageRead[ADC_AVERAGING] = { 0 };
uint16_t adcCurrentRead[ADC_AVERAGING] = { 0 };
uint16_t adcOvervoltage[ADC_AVERAGING] = { 0 };

uint16_t adcVoltageReadAverage = 0;
uint16_t adcCurrentReadAverage = 0;
uint16_t adcOvervoltageAverage = 0;

uint16_t adcVoltageReadAverage_prev = 0;
uint16_t adcCurrentReadAverage_prev = 0;
uint16_t adcOvervoltageAverage_prev = 0;

bool adcVoltageReadAverageChanged = false;
bool adcCurrentReadAverageChanged = false;
bool adcOvervoltageAverageChanged = false;

uint8_t adcSampleIndex = 0;

uint16_t displayVoltageRead;
uint16_t displayCurrentRead;

bool OutputEnabled;

uint8_t initIndicator[9] = {'d', 'n', 'x', 'p', 's', 'u', 'c', 'a', '1'};

void loadSettings();
void initPins();
void processADC();
void setStartupValues(uint16_t voltage, uint16_t current);
void processCalibration();
void processSelfTest();

void setup()
{
	PrintfBegin(&Serial);
	analogReference(EXTERNAL);

	currentOpMode = OpMode::SELF_TEST_START;

	initButtons();
	initPins();

	loadSettings();

	setStartupValues(VOLTAGE_LOWER_LIMIT, CURRENT_UPPER_LIMIT);
	initDAC(VOLTAGE_LOWER_LIMIT, CURRENT_UPPER_LIMIT);
	initDisplay();
}

void loop()
{
	processSelfTest();
	processADC();
	UpdateDisplayRead();
	processCursor();
	processButtons();
	processCalibration();
}

void processSelfTest()
{
	if (currentOpMode == OpMode::SELF_TEST_START)
	{
		if (digitalRead(BUTTON_ENTER_PIN) == LOW && digitalRead(BUTTON_OUTPUT_PIN) == LOW)
		{
			currentOpMode = OpMode::SELF_TEST_COMPLETED;
			return;
		}

		SetDACVoltageRaw((DAC_V_UPPER_LIMIT / 4) * 0);
		selfTestWaitMillis = millis();
		currentOpMode = OpMode::SELF_TEST_PHASE1;

		LCD.clear();
		LCD.setCursor(0, 0);
		LCD.print(PRODUCT_FAMILY);
		LCD.print("-");
		LCD.print(PRODUCT_NAME);
		LCD.print(" SW V");
		LCD.print(FW_MAJOR);
		LCD.print(".");
		LCD.print(FW_MINOR);
		LCD.print(".");
		LCD.print(FW_PATCH);

		displayMessage("Startup self-test", MessageVAlignment::MIDDLE);		
	}
	else if (currentOpMode == OpMode::SELF_TEST_PHASE1 && selfTestWaitMillis + (ADC_POLL_INTERVAL * ADC_AVERAGING * 2) < millis())
	{
		uint16_t measuredVoltage = adcVoltageReadAverage * ADC_VREF * ADC_INPUT_VOLTAGE_DIVIDER / ADC_BITS;
		uint16_t setVoltage = (((DAC_V_UPPER_LIMIT / 4) * 0) * DAC_VREF * GAIN_V_DAC * GAIN_V_CONTROL_LOOP) / DAC_BITS;

		LOG("phase 1 self-test measured: %u, set: %u\r\n", measuredVoltage, setVoltage);

		if (adcCurrentReadAverage < 15)
		{
			LOG("current ok!\r\n");
		}
		else
		{
			lastErrorIndex++;
			errors[lastErrorIndex] = FailureReason::SELF_TEST_PHASE1_CURRENT;
			currentOpMode = OpMode::FAILED;
		}

		if (measuredVoltage >= 0 && measuredVoltage < setVoltage + 50)
		{
			LOG("voltage ok!\r\n");
			SetDACVoltageRaw((DAC_V_UPPER_LIMIT / 4) * 1);
			selfTestWaitMillis = millis();
			currentOpMode = OpMode::SELF_TEST_PHASE2;
			LCD.setCursor(0, 2);
			LCD.write(byte(4));
			LCD.write(byte(4));
			LCD.write(byte(4));
			LCD.write(byte(4));
		}
		else
		{
			lastErrorIndex++;
			errors[lastErrorIndex] = FailureReason::SELF_TEST_PHASE1_VOLTAGE;
			currentOpMode = OpMode::FAILED;
		}
		
	}
	else if (currentOpMode == OpMode::SELF_TEST_PHASE2 && selfTestWaitMillis + (ADC_POLL_INTERVAL * ADC_AVERAGING * 2) < millis())
	{
		uint16_t measuredVoltage = adcVoltageReadAverage * ADC_VREF * ADC_INPUT_VOLTAGE_DIVIDER / ADC_BITS;
		uint16_t setVoltage = (((DAC_V_UPPER_LIMIT / 4) * 1) * DAC_VREF * GAIN_V_DAC * GAIN_V_CONTROL_LOOP) / DAC_BITS;

		LOG("phase 2 self-test measured: %u, set: %u\r\n", measuredVoltage, setVoltage);

		if (adcCurrentReadAverage < 15)
		{
			LOG("current ok!\r\n");
		}
		else
		{
			lastErrorIndex++;
			errors[lastErrorIndex] = FailureReason::SELF_TEST_PHASE2_CURRENT;
			currentOpMode = OpMode::FAILED;
		}

		if (measuredVoltage > setVoltage - 200 && measuredVoltage < setVoltage + 200)
		{
			LOG("ok!\r\n");
			SetDACVoltageRaw((DAC_V_UPPER_LIMIT / 4) * 2);
			selfTestWaitMillis = millis();
			currentOpMode = OpMode::SELF_TEST_PHASE3;
			LCD.setCursor(4, 2);
			LCD.write(byte(4));
			LCD.write(byte(4));
			LCD.write(byte(4));
			LCD.write(byte(4));
		}
		else
		{
			lastErrorIndex++;
			errors[lastErrorIndex] = FailureReason::SELF_TEST_PHASE2_VOLTAGE;
			currentOpMode = OpMode::FAILED;
		}
	}
	else if (currentOpMode == OpMode::SELF_TEST_PHASE3 && selfTestWaitMillis + (ADC_POLL_INTERVAL * ADC_AVERAGING * 2) < millis())
	{
		uint16_t measuredVoltage = adcVoltageReadAverage * ADC_VREF * ADC_INPUT_VOLTAGE_DIVIDER / ADC_BITS;
		uint16_t setVoltage = (((DAC_V_UPPER_LIMIT / 4) * 2) * DAC_VREF * GAIN_V_DAC * GAIN_V_CONTROL_LOOP) / DAC_BITS;

		LOG("phase 3 self-test measured: %u, set: %u\r\n", measuredVoltage, setVoltage);

		if (adcCurrentReadAverage < 15)
		{
			LOG("current ok!\r\n");
		}
		else
		{
			lastErrorIndex++;
			errors[lastErrorIndex] = FailureReason::SELF_TEST_PHASE3_CURRENT;
			currentOpMode = OpMode::FAILED;
		}

		if (measuredVoltage > setVoltage - 200 && measuredVoltage < setVoltage + 200)
		{
			LOG("ok!\r\n");
			SetDACVoltageRaw((DAC_V_UPPER_LIMIT / 4) * 3);
			selfTestWaitMillis = millis();
			currentOpMode = OpMode::SELF_TEST_PHASE4;
			LCD.setCursor(8, 2);
			LCD.write(byte(4));
			LCD.write(byte(4));
			LCD.write(byte(4));
			LCD.write(byte(4));
		}
		else
		{
			lastErrorIndex++;
			errors[lastErrorIndex] = FailureReason::SELF_TEST_PHASE3_VOLTAGE;
			currentOpMode = OpMode::FAILED;
		}
	}
	else if (currentOpMode == OpMode::SELF_TEST_PHASE4 && selfTestWaitMillis + (ADC_POLL_INTERVAL * ADC_AVERAGING * 2) < millis())
	{
		uint16_t measuredVoltage = adcVoltageReadAverage * ADC_VREF * ADC_INPUT_VOLTAGE_DIVIDER / ADC_BITS;
		uint16_t setVoltage = (((DAC_V_UPPER_LIMIT / 4) * 3) * DAC_VREF * GAIN_V_DAC * GAIN_V_CONTROL_LOOP) / DAC_BITS;

		LOG("phase 4 self-test measured: %u, set: %u\r\n", measuredVoltage, setVoltage);

		if (adcCurrentReadAverage < 15)
		{
			LOG("current ok!\r\n");
		}
		else
		{
			lastErrorIndex++;
			errors[lastErrorIndex] = FailureReason::SELF_TEST_PHASE4_CURRENT;
			currentOpMode = OpMode::FAILED;
		}

		if (measuredVoltage > setVoltage - 200 && measuredVoltage < setVoltage + 200)
		{
			LOG("ok!\r\n");
			SetDACVoltageRaw((DAC_V_UPPER_LIMIT / 4) * 4);
			selfTestWaitMillis = millis();
			currentOpMode = OpMode::SELF_TEST_PHASE5;
			LCD.setCursor(12, 2);
			LCD.write(byte(4));
			LCD.write(byte(4));
			LCD.write(byte(4));
			LCD.write(byte(4));
		}
		else
		{
			lastErrorIndex++;
			errors[lastErrorIndex] = FailureReason::SELF_TEST_PHASE4_VOLTAGE;
			currentOpMode = OpMode::FAILED;
		}
	}
	else if (currentOpMode == OpMode::SELF_TEST_PHASE5 && selfTestWaitMillis + (ADC_POLL_INTERVAL * ADC_AVERAGING * 2) < millis())
	{
		uint16_t measuredVoltage = adcVoltageReadAverage * ADC_VREF * ADC_INPUT_VOLTAGE_DIVIDER / ADC_BITS;
		uint16_t setVoltage = (((DAC_V_UPPER_LIMIT / 4) * 4) * DAC_VREF * GAIN_V_DAC * GAIN_V_CONTROL_LOOP) / DAC_BITS;

		LOG("phase 5 self-test measured: %u, set: %u\r\n", measuredVoltage, setVoltage);

		SetDACVoltage(VOLTAGE_LOWER_LIMIT);

		if (adcCurrentReadAverage < 15)
		{
			LOG("current ok!\r\n");
		}
		else
		{
			lastErrorIndex++;
			errors[lastErrorIndex] = FailureReason::SELF_TEST_PHASE5_CURRENT;
			currentOpMode = OpMode::FAILED;
		}

		if (measuredVoltage > setVoltage - 1000 && measuredVoltage < setVoltage + 200)
		{
			LOG("ok!\r\n");
			currentOpMode = OpMode::SELF_TEST_COMPLETED;
		}
		else
		{
			lastErrorIndex++;
			errors[lastErrorIndex] = FailureReason::SELF_TEST_PHASE5_VOLTAGE;
			currentOpMode = OpMode::FAILED;
		}
	}
	else if (currentOpMode == OpMode::SELF_TEST_COMPLETED)
	{
		currentOpMode = OpMode::REGULAR;
		LCD.setCursor(16, 2);
		LCD.write(byte(4));
		LCD.write(byte(4));
		LCD.write(byte(4));
		LCD.write(byte(4));
		delay((ADC_POLL_INTERVAL * ADC_AVERAGING * 2));
		displayMainScreen();
		UpdateDisplay();
	}
	else if (currentOpMode == OpMode::FAILED)
	{
		SetDACVoltage(DAC_V_LOWER_LIMIT);
		digitalWrite(REL_OUT_PIN, LOW);
		digitalWrite(LED_OUTPUT_PIN, LOW);

		LCD.setCursor(0, 2);
		LCD.print("                    ");
		LCD.setCursor(7, 2);
		LCD.print("FAILED");
		LCD.setCursor(0, 3);
		LCD.print("Error(s): ");

		if (lastErrorIndex == -1 || lastErrorIndex >= static_cast<int8_t>(sizeof(errors)))
		{
			LCD.print("unknown");
		}
		else
		{
			for (uint8_t n = 0; n <= lastErrorIndex; n++)
			{
				LCD.print(static_cast<uint8_t>(errors[n]));
				if (n != lastErrorIndex)
					LCD.print(", ");
			}
		}

		while(1)
		{
			digitalWrite(LED_STATUS_PIN, HIGH);
			delay(300);
			digitalWrite(LED_STATUS_PIN, LOW);
			delay(300);
		}		

	}
}

void loadSettings()
{
	// load init indicator from EEPROM
	uint8_t bufCompareInitIndicator[9];
	EEPROM.get(EEPROM_ADDR_INIT_INDICATOR, bufCompareInitIndicator);
	
	// load settings if init indicator matches, otherwise save default settings and init indicator to EEPROM
	if (memcmp(bufCompareInitIndicator, initIndicator, sizeof(bufCompareInitIndicator)) == 0)
	{
		EEPROM.get(EEPROM_ADDR_CALIBRATION, calibrationValues);
	}
	else
	{
		EEPROM.put(EEPROM_ADDR_CALIBRATION, calibrationValues);
		EEPROM.put(EEPROM_ADDR_INIT_INDICATOR, initIndicator);
	}	
}

void processCalibration()
{
	if (currentOpMode != OpMode::CALIBRATION_START &&
		currentOpMode != OpMode::CALIBRATING &&
		currentOpMode != OpMode::CALIBRATING_WAIT_SETTLE && 
		currentOpMode != OpMode::CALIBRATION_FINISHED_WAITING && 
		currentOpMode != OpMode::CALIBRATION_FINISHED_EXIT)
		{
			return;
		}
		

	if (currentCalibrationStep == DAC_V_STEPS && currentOpMode == OpMode::CALIBRATING)	// finished
	{
		LOG("Calibration completed\r\n");
		for (uint16_t n = 0; n < DAC_V_STEPS; n++)
		{
			Serial.print(calibrationValues[n]);
			Serial.print(", ");
		}
		Serial.println("");
		SetDACVoltage(VoltageMenu_E.Digit * 1000 + VoltageMenu_z.Digit * 100);
		LCD.clear();
		displayMessage("Calibration finished\r\nPress any key\r\nto continue", MessageVAlignment::MIDDLE);
		currentOpMode = OpMode::CALIBRATION_FINISHED_WAITING;
	}
	else if (currentCalibrationStep == DAC_V_STEPS && currentOpMode == OpMode::CALIBRATION_FINISHED_WAITING)
	{

	}
	else if (currentCalibrationStep == DAC_V_STEPS && currentOpMode == OpMode::CALIBRATION_FINISHED_EXIT)
	{
		EEPROM.put(EEPROM_ADDR_CALIBRATION, calibrationValues);
		currentOpMode = OpMode::REGULAR;
		currentCalibrationStep = 0;
		SelectedMenu = &VoltageMenu;
		displayMainScreen();
	}
	else
	{
		if (currentOpMode == OpMode::CALIBRATION_START)
		{
			LOG("CALIBRATION_START: set DAC to lower limit\r\n");
			LOG("CALIBRATING_START: wait for output voltage to settle\r\n");
			SetDACVoltageRaw(DAC_V_LOWER_LIMIT);
			currentCalibrationStep = 0;
			currentOpMode = OpMode::CALIBRATING_WAIT_SETTLE;
			calibrationADCwaitMillis = millis();
		}
		else if (currentOpMode == OpMode::CALIBRATING_WAIT_SETTLE)
		{
			if (calibrationADCwaitMillis + (ADC_POLL_INTERVAL * ADC_AVERAGING * 8) < millis())
			{
				LOG("CALIBRATING_WAIT_SETTLE: done waiting\r\n");
				currentOpMode = OpMode::CALIBRATING;
				calibrationADCwaitMillis = millis();
			}
		}
		else if (currentOpMode == OpMode::CALIBRATING)
		{
			// if (calibrationADCwaitMillis + (ADC_POLL_INTERVAL) < millis())
			if (calibrationADCwaitMillis + (ADC_POLL_INTERVAL * ADC_AVERAGING * 3) < millis())
			{
				calibrationValues[currentCalibrationStep] = adcCurrentReadAverage;
				calibrationPercentage = (currentCalibrationStep * 100) / (DAC_V_STEPS - 1);
				if (calibrationPercentage != calibrationPercentage_prev)
				{
					calibrationPercentage_prev = calibrationPercentage;
					LCD.setCursor(9, 3);
					LCD.print(calibrationPercentage);
					LCD.print("%");
				}
				LOG("CALIBRATING: [%03u%%] index %u, val %u\r\n", calibrationPercentage, currentCalibrationStep, adcCurrentReadAverage);

				SetDACVoltageRaw(DAC_V_LOWER_LIMIT + (DAC_V_STEPSIZE * currentCalibrationStep));
				currentCalibrationStep++;
				calibrationADCwaitMillis = millis();
			}
		}
	}
}

void initPins()
{
	pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
	pinMode(BUTTON_ENTER_PIN, INPUT_PULLUP);
	pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
	pinMode(BUTTON_OUTPUT_PIN, INPUT_PULLUP);
	
	pinMode(LED_STATUS_PIN, OUTPUT);
	pinMode(LED_OUTPUT_PIN, OUTPUT);

	digitalWrite(LED_STATUS_PIN, LOW);
	digitalWrite(LED_OUTPUT_PIN, LOW);
	
	pinMode(REL_OUT_PIN, OUTPUT);
	digitalWrite(REL_OUT_PIN, LOW);
}

void setStartupValues(uint16_t voltage, uint16_t current)
{
	VoltageMenu_E.Digit = voltage / 1000;
	VoltageMenu_z.Digit = (voltage / 100) % 10;

	CurrentMenu_E.Digit = current / 1000;
	CurrentMenu_z.Digit = (current / 100) % 10;
	CurrentMenu_h.Digit = (current / 10) % 10;
	CurrentMenu_t.Digit = current % 10;
}



