#pragma once

#include <Arduino.h>
#include <config.h>

typedef enum class MenuType : uint8_t
{
	MAIN_SCREEN =		0U,
	MAIN_SCREEN_VAL	=	1U,
	CONFIG_MENU =		2U,
	CONFIG_MENU_VAL =	3U,
	MESSAGE =			4U
} MenuType_t;

typedef struct HomeMenuItem
{
	// bool IsSubmenu;
	// bool IsTextMenu;
	MenuType_t type;
	char Caption[LCD_COLS * LCD_ROWS + ((LCD_ROWS - 1) * 2) + 1];	// Rows * Columns, plus Rows - 1 * 2 for \r\n + 1 for terminating 0 character
	bool Commit;
	// bool DisplayUpdated;	// obsolete

	uint8_t CursorX;
	uint8_t CursorY;

	int8_t Digit;

	int8_t DigitMin;
	int8_t DigitMax;
	
	HomeMenuItem *ButtonUp;
	HomeMenuItem *ButtonEnter;
	HomeMenuItem *ButtonDown;
} HomeMenuItem_t;

extern HomeMenuItem_t VoltageMenu;
extern HomeMenuItem_t VoltageMenu_E;
extern HomeMenuItem_t VoltageMenu_z;

extern HomeMenuItem_t CurrentMenu;
extern HomeMenuItem_t CurrentMenu_E;
extern HomeMenuItem_t CurrentMenu_z;
extern HomeMenuItem_t CurrentMenu_h;
extern HomeMenuItem_t CurrentMenu_t;

extern HomeMenuItem_t ConfigMenu;
extern HomeMenuItem_t ConfigMenu_Calibrate;
extern HomeMenuItem_t ConfigMenu_Message_Calibrating;
extern HomeMenuItem_t ConfigMenu_Exit;

extern HomeMenuItem_t *SelectedMenu;