// MenuOption.h

#ifndef _MENUOPTION_h
#define _MENUOPTION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Menu.h"

enum SelectAction
{
  Edit,
  OpenMenu
};

class MenuOption
{

private:
  char* displayName;
  SelectAction selectAction;
  void* value;
  void* valueOptions[];
  Menu menuToOpen;

};
  


#endif

