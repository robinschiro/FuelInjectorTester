// Configuration.h

#ifndef _CONFIGURATION_h
#define _CONFIGURATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Configuration
{
private:
	int injectorsToRun[];
  int numInjectors;
  int rpm;
  int injectionTimeInMicroseconds;
  int timeToRunInSeconds;
};

#endif

