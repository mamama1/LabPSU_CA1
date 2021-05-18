///////////////////////////////////////////////////////////////////
///   devnetX Button Debouncer Library for Arduino, Ver. 2.1    ///
///   -------------------------------------------------------   ///
///   Author: Alexej Goujine                                    ///
///   Copyright (c) 2011-2021, devnetX http://www.devnetx.at    ///
///   E-Mail: dev@devnetx.at                                    ///
///   Last changed on: 19.03.2021 (DD.MM.YYYY)                  ///
///   -------------------------------------------------------   ///
///   This code is provided 'as is', use it at your own risk.   ///
///////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <ButtonDebouncer.h>

ButtonDebouncer::ButtonDebouncer(void)
{
	uint8_t n;

	for (n = 0; n < BUTTON_DEBOUNCER_BUTTON_COUNT; n++)
	{
		this->ButtonTime[n] = 0;
		this->ButtonState[n] = BDButtonState::IDLE;
		this->ButtonPressed[n] = false;
		this->ButtonRepeatIntervalTime[n] = BD_DEFAULT_REPEAT_INTERVAL_TIME;
	}

	this->ButtonEventsMask = BDButtonState::IDLE;

	this->Parent = NULL;
	this->ButtonPressFunction = NULL;
	this->ButtonHoldFunction = NULL;
	this->ButtonRepeatFunction = NULL;
	this->ButtonPressDoneFunction = NULL;
	this->ButtonRepeatDoneFunction = NULL;
	this->ButtonReleasedFunction = NULL;
}

void ButtonDebouncer::Begin(void *parent, BDButtonState_t buttonEventsMask, uint16_t RepeatIntervalTime)
{
	this->Parent = parent;
	this->ButtonEventsMask = buttonEventsMask;

	for (uint8_t n = 0; n < BUTTON_DEBOUNCER_BUTTON_COUNT; n++)
	{
		this->ButtonRepeatIntervalTime[n] = RepeatIntervalTime;
	}
}

bool ButtonDebouncer::IsButtonPressed(uint8_t buttonID)
{
	return this->ButtonPressed[buttonID];
}

bool ButtonDebouncer::IsButtonReleased(uint8_t buttonID)
{
	return !IsButtonPressed(buttonID);
}

void ButtonDebouncer::SetRepeatIntervalTime(uint8_t buttonID, uint16_t interval)
{
	ButtonRepeatIntervalTime[buttonID] = interval;
}

bool ButtonDebouncer::Process(uint8_t buttonID, bool active)
{
	uint32_t currentMillis = millis();

	// Check button state and handle it
	if (active == true)
	{
		if (this->ButtonState[buttonID] == BDButtonState::IDLE)
		{
			// Button is now pressed, debouncing it
			this->ButtonState[buttonID] = BDButtonState::PRESSED;
			this->ButtonTime[buttonID] = currentMillis;
			this->ButtonPressed[buttonID] = true;
			return false;
		}
		else if ((this->ButtonState[buttonID] == BDButtonState::PRESSED) && (currentMillis - this->ButtonTime[buttonID] > BD_DEBOUNCE_PRESS_TIME))
		{
			// Button is pressed and debounced
			this->ButtonState[buttonID] = BDButtonState::HOLD;
			this->ButtonTime[buttonID] = currentMillis;

			if (this->ButtonPressFunction != NULL)
				(*ButtonPressFunction)(this->Parent, buttonID);

			return this->ButtonEventsMask & BDButtonState::PRESSED;
		}
		else if ((this->ButtonState[buttonID] == BDButtonState::HOLD) && (currentMillis - this->ButtonTime[buttonID] > BD_HOLD_TIME))
		{
			// Button is pressed and hold
			this->ButtonState[buttonID] = BDButtonState::REPEAT;
			this->ButtonTime[buttonID] = currentMillis;

			if (this->ButtonHoldFunction != NULL)
				(*ButtonHoldFunction)(this->Parent, buttonID);

			return this->ButtonEventsMask & BDButtonState::HOLD;
		}
		else if ((this->ButtonState[buttonID] == BDButtonState::REPEAT) && (currentMillis - this->ButtonTime[buttonID] > ButtonRepeatIntervalTime[buttonID]))
		{
			// Button is pressed and hold: repeat button
			this->ButtonTime[buttonID] = currentMillis;

			if (this->ButtonRepeatFunction != NULL)
				(*ButtonRepeatFunction)(this->Parent, buttonID);

			return this->ButtonEventsMask & BDButtonState::REPEAT;
		}
	}
	else
	{
		if (this->ButtonState[buttonID] == BDButtonState::PRESSED)
		{
			// Button was released during debounce. Debounce the release
			this->ButtonState[buttonID] = BDButtonState::RELEASED;
			this->ButtonTime[buttonID] = currentMillis;

			return false;
		}
		else if (this->ButtonState[buttonID] == BDButtonState::HOLD)
		{
			// Button is now released. Debounce it
			this->ButtonState[buttonID] = BDButtonState::RELEASED;
			this->ButtonTime[buttonID] = currentMillis;

			if (this->ButtonPressDoneFunction != NULL)
				(*ButtonPressDoneFunction)(this->Parent, buttonID);

			return false;
		}
		else if (this->ButtonState[buttonID] == BDButtonState::REPEAT)
		{
			// Button is now released. Debounce it
			this->ButtonState[buttonID] = BDButtonState::RELEASED;
			this->ButtonTime[buttonID] = currentMillis;

			if (this->ButtonRepeatDoneFunction != NULL)
				(*ButtonRepeatDoneFunction)(this->Parent, buttonID);
			
			return false;
		}
		else if ((this->ButtonState[buttonID] == BDButtonState::RELEASED) && (currentMillis - this->ButtonTime[buttonID] > BD_DEBOUNCE_RELEASE_TIME))
		{
			// Button released and debounced
			this->ButtonState[buttonID] = BDButtonState::IDLE;
			
			if (!this->ButtonPressed[buttonID])
				return false;

			this->ButtonPressed[buttonID] = false;

			if (this->ButtonReleasedFunction != NULL)
				(*ButtonReleasedFunction)(this->Parent, buttonID);

			return this->ButtonEventsMask & BDButtonState::RELEASED;
		}
	}

	return false;
}

void ButtonDebouncer::OnButtonPress(void (*func)(void *parent, uint8_t buttonID))
{
	this->ButtonPressFunction = func;
}

void ButtonDebouncer::OnButtonHold(void (*func)(void *parent, uint8_t buttonID))
{
	this->ButtonHoldFunction = func;
}

void ButtonDebouncer::OnButtonRepeat(void (*func)(void *parent, uint8_t buttonID))
{
	this->ButtonRepeatFunction = func;
}

void ButtonDebouncer::OnButtonPressDone(void (*func)(void *parent, uint8_t buttonID))
{
	this->ButtonPressDoneFunction = func;
}

void ButtonDebouncer::OnButtonRepeatDone(void (*func)(void *parent, uint8_t buttonID))
{
	this->ButtonRepeatDoneFunction = func;
}

void ButtonDebouncer::OnButtonReleased(void (*func)(void *parent, uint8_t buttonID))
{
	this->ButtonReleasedFunction = func;	
}
