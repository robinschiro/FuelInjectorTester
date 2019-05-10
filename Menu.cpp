// 
// 
// 

#include "Menu.h"

MenuClass::MenuClass(LiquidCrystal* inputLcd) : lcd(inputLcd)
{
}

void MenuClass::OnInput(int LcdKey)
{
}

void MenuClass::DisplayMenu()
{
	this->PrintToLine("Hello World!", 0);
}

void MenuClass::PrintToLine(char* data, int lineNumber)
{
	this->lcd->setCursor(0, lineNumber);

	char dataFormatted[17];
	sprintf(dataFormatted, "%-16s", data);

	this->lcd->print(dataFormatted);
}

//MenuClass Menu;