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

#pragma once

#include <Arduino.h>
#include <RedirectPrintf.h>

#define BUTTON_DEBOUNCER_BUTTON_COUNT		1U
#define BD_DEFAULT_REPEAT_INTERVAL_TIME		3UL

typedef enum class BDButtonState : uint8_t
{
	IDLE =			0U,
	PRESSED	=		1U,
	HOLD =			2U,
	REPEAT =		4U,
	RELEASED =		8U
} BDButtonState_t;

typedef void (*TButtonFunction)(void *parent, uint8_t buttonID);

class ButtonDebouncer
{
	public:
		/// Construct a new ButtonDebouncer object
		/// 
		ButtonDebouncer(void);

		void Begin(void *parent, BDButtonState_t buttonEventsMask, uint16_t RepeatIntervalTime = BD_DEFAULT_REPEAT_INTERVAL_TIME);

		void OnButtonPress(void (*func)(void *parent, uint8_t buttonID));
		void OnButtonHold(void (*func)(void *parent, uint8_t buttonID));
		void OnButtonRepeat(void (*func)(void *parent, uint8_t buttonID));
		void OnButtonPressDone(void (*func)(void *parent, uint8_t buttonID));
		void OnButtonRepeatDone(void (*func)(void *parent, uint8_t buttonID));
		void OnButtonReleased(void (*func)(void *parent, uint8_t buttonID));

		/// Process the button
		/// 
		/// @param buttonID ID of the button
		/// @param active bool button pin (positive logic)
		/// @return true A callback was raised
		/// @return false No action
		bool Process(uint8_t buttonID, bool active);
		
		/// Current button pressed state
		/// 
		/// @param buttonID ID of the button
		/// @return true Button is pressed
		/// @return false Button is not pressed
		bool IsButtonPressed(uint8_t buttonID);

		/// Current button released state
		/// 
		/// @param buttonID ID of the button
		/// @return true Button is pressed
		/// @return false Button is not pressed
		bool IsButtonReleased(uint8_t buttonID);

		void SetRepeatIntervalTime(uint8_t buttonID, uint16_t interval);

	private:

		#define BD_DEBOUNCE_PRESS_TIME				25UL
		#define BD_HOLD_TIME						250UL
		#define BD_DEBOUNCE_RELEASE_TIME			25UL

		BDButtonState_t	ButtonEventsMask;

		bool 			ButtonPressed[BUTTON_DEBOUNCER_BUTTON_COUNT];
		BDButtonState_t ButtonState[BUTTON_DEBOUNCER_BUTTON_COUNT];
		uint32_t		ButtonTime[BUTTON_DEBOUNCER_BUTTON_COUNT];
		uint16_t		ButtonRepeatIntervalTime[BUTTON_DEBOUNCER_BUTTON_COUNT];

		void *Parent;
		
		void (*ButtonPressFunction)(void *parent, uint8_t buttonID);
		void (*ButtonHoldFunction)(void *parent, uint8_t buttonID);
		void (*ButtonRepeatFunction)(void *parent, uint8_t buttonID);
		void (*ButtonPressDoneFunction)(void *parent, uint8_t buttonID);
		void (*ButtonRepeatDoneFunction)(void *parent, uint8_t buttonID);
		void (*ButtonReleasedFunction)(void *parent, uint8_t buttonID);
};

constexpr inline BDButtonState operator| (const BDButtonState x, const BDButtonState y)
{
 	return (BDButtonState)((uint8_t)x | (uint8_t)y);
}

constexpr inline bool operator& (const BDButtonState x, const BDButtonState y)
{
 	return ((uint8_t)x & (uint8_t)y);
}