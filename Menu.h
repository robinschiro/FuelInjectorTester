// Menu.h

#ifndef _MENU_h
#define _MENU_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
	#include <LiquidCrystal.h>
#else
	#include "WProgram.h"
#endif

class MenuClass
{
	protected:
		void PrintToLine(char* data, int lineNumber);
		

	public:
		void OnInput(int LcdKey);
		void DisplayMenu();
		MenuClass(LiquidCrystal* inputLcd);

	private:
		LiquidCrystal* lcd;
};

//extern MenuClass Menu;

#endif

