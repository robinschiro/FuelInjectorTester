// Configuration.h

#ifndef _CONFIGURATION_h
#define _CONFIGURATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class ConfigurationClass
{
protected:
	int injectorsToRun[];

public:
	void Init();

};

extern ConfigurationClass Configuration;

#endif

