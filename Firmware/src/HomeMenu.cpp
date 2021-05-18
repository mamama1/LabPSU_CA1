#include <Arduino.h>
#include <HomeMenu.h>

/*
     Set|   Read|
> 0.000V| 0.000V| CC
  0.000A| 0.000A| CV
  0.000W| 0.000W| 

*/

HomeMenuItem_t *SelectedMenu = &VoltageMenu;

HomeMenuItem_t VoltageMenu = {
	// .IsSubmenu = false,
	// .IsTextMenu = false,
	.type = MenuType::MAIN_SCREEN,
	.Caption = { 0, },
	.Commit = false,
	.CursorX = 0,
	.CursorY = 1,
	.Digit = 0,
	.DigitMin = 0,
	.DigitMax = 0,
	.ButtonUp = &ConfigMenu,
	.ButtonEnter = &VoltageMenu_E,
	.ButtonDown = &CurrentMenu
};

	HomeMenuItem_t VoltageMenu_E = {
		// .IsSubmenu = true,
		// .IsTextMenu = false,
		.type = MenuType::MAIN_SCREEN_VAL,
		.Caption = { 0, },
		.Commit = false,
		.CursorX = 2,
		.CursorY = 1,
		.Digit = 0,
		.DigitMin = 0,
		.DigitMax = 25,
		.ButtonUp = &VoltageMenu_E,
		.ButtonEnter = &VoltageMenu_z,
		.ButtonDown = &VoltageMenu_E
	};

	HomeMenuItem_t VoltageMenu_z = {
		// .IsSubmenu = true,
		// .IsTextMenu = false,
		.type = MenuType::MAIN_SCREEN_VAL,
		.Caption = { 0, },
		.Commit = true,
		.CursorX = 4,
		.CursorY = 1,
		.Digit = 0,
		.DigitMin = 0,
		.DigitMax = 9,
		.ButtonUp = &VoltageMenu_z,
		.ButtonEnter = &VoltageMenu,
		.ButtonDown = &VoltageMenu_z
	};

HomeMenuItem_t CurrentMenu = {
	// .IsSubmenu = false,
	// .IsTextMenu = false,
	.type = MenuType::MAIN_SCREEN,
	.Caption = { 0, },
	.Commit = false,
	.CursorX = 0,
	.CursorY = 2,
	.Digit = 0,
	.DigitMin = 0,
	.DigitMax = 0,
	.ButtonUp = &VoltageMenu,
	.ButtonEnter = &CurrentMenu_E,
	.ButtonDown = &ConfigMenu
};

	HomeMenuItem_t CurrentMenu_E = {
		// .IsSubmenu = true,
		// .IsTextMenu = false,
		.type = MenuType::MAIN_SCREEN_VAL,
		.Caption = { 0, },
		.Commit = false,
		.CursorX = 2,
		.CursorY = 2,
		.Digit = 0,
		.DigitMin = 0,
		.DigitMax = 1,
		.ButtonUp = &CurrentMenu_E,
		.ButtonEnter = &CurrentMenu_z,
		.ButtonDown = &CurrentMenu_E
	};

	HomeMenuItem_t CurrentMenu_z = {
		// .IsSubmenu = true,
		// .IsTextMenu = false,
		.type = MenuType::MAIN_SCREEN_VAL,
		.Caption = { 0, },
		.Commit = false,
		.CursorX = 4,
		.CursorY = 2,
		.Digit = 0,
		.DigitMin = 0,
		.DigitMax = 9,
		.ButtonUp = &CurrentMenu_z,
		.ButtonEnter = &CurrentMenu_h,
		.ButtonDown = &CurrentMenu_z
	};

	HomeMenuItem_t CurrentMenu_h = {
		// .IsSubmenu = true,
		// .IsTextMenu = false,
		.type = MenuType::MAIN_SCREEN_VAL,
		.Caption = { 0, },
		.Commit = false,
		.CursorX = 5,
		.CursorY = 2,
		.Digit = 0,
		.DigitMin = 0,
		.DigitMax = 9,
		.ButtonUp = &CurrentMenu_h,
		.ButtonEnter = &CurrentMenu_t,
		.ButtonDown = &CurrentMenu_h
	};

	HomeMenuItem_t CurrentMenu_t = {
		// .IsSubmenu = true,
		// .IsTextMenu = false,
		.type = MenuType::MAIN_SCREEN_VAL,
		.Caption = { 0, },
		.Commit = true,
		.CursorX = 6,
		.CursorY = 2,
		.Digit = 0,
		.DigitMin = 0,
		.DigitMax = 9,
		.ButtonUp = &CurrentMenu_t,
		.ButtonEnter = &CurrentMenu,
		.ButtonDown = &CurrentMenu_t
	};

HomeMenuItem_t ConfigMenu = {
	// .IsSubmenu = false,
	// .IsTextMenu = false,
	.type = MenuType::MAIN_SCREEN,
	.Caption = { 0, },
	.Commit = false,
	.CursorX = 0,
	.CursorY = 0,
	.Digit = 0,
	.DigitMin = 0,
	.DigitMax = 0,
	.ButtonUp = &CurrentMenu,
	.ButtonEnter = &ConfigMenu_Calibrate,
	.ButtonDown = &VoltageMenu
};

	HomeMenuItem_t ConfigMenu_Calibrate = {
		// .IsSubmenu = true,
		// .IsTextMenu = true,
		.type = MenuType::CONFIG_MENU,
		{.Caption = "Calibrate"},
		.Commit = false,
		.CursorX = 0,
		.CursorY = 0,
		.Digit = 0,
		.DigitMin = 0,
		.DigitMax = 0,
		.ButtonUp = &ConfigMenu_Exit,
		.ButtonEnter = &ConfigMenu_Message_Calibrating,
		.ButtonDown = &ConfigMenu_Exit
	};
		HomeMenuItem_t ConfigMenu_Message_Calibrating = {
			// .IsSubmenu = true,
			// .IsTextMenu = true,
			.type = MenuType::MESSAGE,
			{.Caption = ">> Calibrating <<\r\nKeep output dis-\r\nconnected"},
			.Commit = false,
			.CursorX = 0,
			.CursorY = 0,
			.Digit = 0,
			.DigitMin = 0,
			.DigitMax = 0,
			.ButtonUp = NULL,
			.ButtonEnter = NULL,
			.ButtonDown = NULL
		};

	HomeMenuItem_t ConfigMenu_Exit = {
		// .IsSubmenu = true,
		// .IsTextMenu = true,
		.type = MenuType::CONFIG_MENU,
		{.Caption = "Exit"},
		.Commit = false,
		.CursorX = 0,
		.CursorY = 1,
		.Digit = 0,
		.DigitMin = 0,
		.DigitMax = 0,
		.ButtonUp = &ConfigMenu_Calibrate,
		.ButtonEnter = &VoltageMenu,
		.ButtonDown = &ConfigMenu_Calibrate
	};