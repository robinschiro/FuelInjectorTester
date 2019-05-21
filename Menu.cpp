// 
// 
// 

#include "Menu.h"

Menu::Menu(LiquidCrystal* inputLcd, Configuration* inputConfig) : lcd(inputLcd), config(inputConfig)
{
}

void Menu::OnInput(int LcdKey)
{
}

void Menu::DisplayMenu()
{
	this->PrintToLine("Hello World!", 0);
}

void Menu::PrintToLine(char* data, int lineNumber)
{
	this->lcd->setCursor(0, lineNumber);

	char dataFormatted[17];
	sprintf(dataFormatted, "%-16s", data);

	this->lcd->print(dataFormatted);
}

//Menu Menu;
