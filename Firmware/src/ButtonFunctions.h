#pragma once

#include <Arduino.h>

extern HomeMenuItem_t *SelectedMenu;

extern ButtonDebouncer ButtonUp;
extern ButtonDebouncer ButtonEnter;
extern ButtonDebouncer ButtonDown;
extern ButtonDebouncer ButtonOutput;

extern bool OutputEnabled;

bool handleMessages()
{
	if (SelectedMenu->type == MenuType::MESSAGE && currentOpMode == OpMode::CALIBRATION_FINISHED_WAITING)
	{
		LOG("OpMode::CALIBRATION_FINISHED_EXIT\r\n");
		currentOpMode = OpMode::CALIBRATION_FINISHED_EXIT;
		return true;
	}
	else if (SelectedMenu->type == MenuType::MESSAGE)
	{
		return true;
	}		

	return false;
}

void ButtonUp_Action()
{
	if (handleMessages())
		return;

	if (SelectedMenu->type == MenuType::MAIN_SCREEN_VAL || SelectedMenu->type == MenuType::CONFIG_MENU_VAL)
	{
		SelectedMenu->Digit++;

		if (SelectedMenu->Digit > SelectedMenu->DigitMax)
			SelectedMenu->Digit = SelectedMenu->DigitMin;
	}
	else
	{
		SelectedMenu = SelectedMenu->ButtonUp;
	}

	UpdateDisplay();
}

void ButtonEnter_Action()
{
	if (handleMessages())
		return;

	if (SelectedMenu == &VoltageMenu_E && SelectedMenu->Digit == 25)
	{
		VoltageMenu_z.Digit = 0;

		SelectedMenu = &VoltageMenu_z;
		UpdateDisplay();
	}
	else if (SelectedMenu == &CurrentMenu_E && SelectedMenu->Digit == 1)
	{
		CurrentMenu_z.Digit = 0;
		CurrentMenu_h.Digit = 0;
		CurrentMenu_t.Digit = 0;
	
		SelectedMenu = &CurrentMenu_z;
		UpdateDisplay();
		SelectedMenu = &CurrentMenu_h;
		UpdateDisplay();
		SelectedMenu = &CurrentMenu_t;
		UpdateDisplay();
	}

	if (SelectedMenu == &ConfigMenu)
	{
		displayConfigMenu();
	}

	if (SelectedMenu == &ConfigMenu_Exit)
	{
		displayMainScreen();	// draw main screen
	}

	if (SelectedMenu->ButtonEnter != NULL)
	{
		SelectedMenu = SelectedMenu->ButtonEnter;
	}

	if (SelectedMenu == &ConfigMenu_Message_Calibrating)
	{
		LCD.clear();
		displayMessage(SelectedMenu->Caption, MessageVAlignment::MIDDLE);
		currentOpMode = OpMode::CALIBRATION_START;
	}

	if (SelectedMenu == &VoltageMenu)
	{
		SetDACVoltage(VoltageMenu_E.Digit * 1000 + VoltageMenu_z.Digit * 100);
		updateDisplaySetWattage();
	}

	if (SelectedMenu == &CurrentMenu)
	{
		SetDACCurrent(CurrentMenu_E.Digit * 1000 + CurrentMenu_z.Digit * 100 + CurrentMenu_h.Digit * 10 + CurrentMenu_t.Digit * 1);
		updateDisplaySetWattage();
	}

	

	UpdateDisplay();
}

void ButtonDown_Action()
{
	if (handleMessages())
		return;

	if (SelectedMenu->type == MenuType::MAIN_SCREEN_VAL || SelectedMenu->type == MenuType::CONFIG_MENU_VAL)
	{
		SelectedMenu->Digit--;
		
		if (SelectedMenu->Digit < SelectedMenu->DigitMin)
			SelectedMenu->Digit = SelectedMenu->DigitMax;
	}
	else
	{
		SelectedMenu = SelectedMenu->ButtonDown;
	}

	UpdateDisplay();
}

void ButtonOutput_Action()
{
	if (handleMessages())
		return;

	OutputEnabled = !OutputEnabled;

	digitalWrite(REL_OUT_PIN, OutputEnabled);
	digitalWrite(LED_OUTPUT_PIN, OutputEnabled);
}

void initButtons()
{
	ButtonUp.Begin(NULL, 	 BDButtonState::PRESSED | BDButtonState::REPEAT);
	ButtonEnter.Begin(NULL,  BDButtonState::PRESSED | BDButtonState::HOLD);
	ButtonDown.Begin(NULL,	 BDButtonState::PRESSED | BDButtonState::REPEAT);
	ButtonOutput.Begin(NULL, BDButtonState::PRESSED);
	
	// ButtonUp
	ButtonUp.OnButtonPress([](void *, uint8_t buttonID) {
		LOG("ButtonUp_OnButtonPress!\r\n");
		ButtonUp_Action();
	});

	ButtonUp.OnButtonRepeat([](void *, uint8_t buttonID) {
		LOG("ButtonUp_OnButtonRepeat!\r\n");
		ButtonUp_Action();
	});

	// ButtonEnter
	ButtonEnter.OnButtonPress([](void *, uint8_t buttonID) {
		LOG("ButtonEnter_OnButtonPress!\r\n");
		ButtonEnter_Action();
	});

	ButtonEnter.OnButtonHold([](void *, uint8_t buttonID) {
		LOG("ButtonEnter_OnButtonHold!\r\n");
	});

	// ButtonDown
	ButtonDown.OnButtonPress([](void *, uint8_t buttonID) {
		LOG("ButtonDown_OnButtonPress!\r\n");
		ButtonDown_Action();
	});

	ButtonDown.OnButtonRepeat([](void *, uint8_t buttonID) {
		LOG("ButtonDown_OnButtonRepeat!\r\n");
		ButtonDown_Action();
	});

	// Button Output
	ButtonOutput.OnButtonPress([](void *, uint8_t buttonID) {
		LOG("ButtonOutput_OnButtonPress!\r\n", buttonID);

		ButtonOutput_Action();
	});
}

void processButtons()
{
	if (currentOpMode != OpMode::REGULAR &&
		currentOpMode != OpMode::CALIBRATION_FINISHED_WAITING)
		return;

	if (SelectedMenu->type == MenuType::MAIN_SCREEN_VAL || SelectedMenu->type == MenuType::CONFIG_MENU_VAL)
	{
		ButtonUp.SetRepeatIntervalTime(0, BUTTON_REPEAT_VALUE_INTERVAL);
		ButtonDown.SetRepeatIntervalTime(0, BUTTON_REPEAT_VALUE_INTERVAL);
	}
	else
	{
		ButtonUp.SetRepeatIntervalTime(0, BUTTON_REPEAT_MENU_INTERVAL);
		ButtonDown.SetRepeatIntervalTime(0, BUTTON_REPEAT_MENU_INTERVAL);
	}
		
	// Check if we captured one of our specified events from at least one button
	ButtonUp.Process(0, 	!digitalRead(BUTTON_UP_PIN));
	ButtonEnter.Process(0,	!digitalRead(BUTTON_ENTER_PIN));
	ButtonDown.Process(0,	!digitalRead(BUTTON_DOWN_PIN));
	ButtonOutput.Process(0,	!digitalRead(BUTTON_OUTPUT_PIN));
}