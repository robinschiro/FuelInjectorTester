// Menu.h

#ifndef _MENU_h
#define _MENU_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
	#include <LiquidCrystal.h>
#else
	#include "WProgram.h"
#endif

#include "Configuration.h"

class Menu
{
	protected:
		void PrintToLine(char* data, int lineNumber);

	public:
		Menu(LiquidCrystal* inputLcd, Configuration* config);
		
		void OnInput(int LcdKey);
		void DisplayMenu();		

	private:
		LiquidCrystal* lcd;
    Configuration* config;
};

#endif

