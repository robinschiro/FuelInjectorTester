/* Fuel Injector Tester
 * NOTE: Possible pins to control fuel injector are 2, 3, 11, 12, and 13 (use these numbers in code)
 * These correspond to physical pins 3, 4, 5, 6 and 7 on the LCD Keypad Shield (respectively)
 */

#include <LiquidCrystal.h>
#include <pt.h>


/* Constants */
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
#define MAX_NUM_INJECTORS 6
#define POT_PIN 5
#define LARGE_RPM_INCREMENT 100
#define SMALL_RPM_INCREMENT 10
#define RPM_MAX 8000
#define RPM_MIN 0
#define DELAY_MIN 10
#define DELAY_MAX 15000
#define INJECTION_TIME_MAX 12
#define INJECTION_TIME_MIN 2
#define INJECTION_TIME_INCREMENT 1

/* Enums */
typedef enum _Mode
{
	DriveInjectors,
	DriveInjectorsAuto,
	DriveUI
} Mode;

/* Globals */

// Instantiate LCD panel struct with selected pins.
LiquidCrystal LCD(8, 9, 4, 5, 6, 7);

// Create array of pins to be used for fuel injectors.
// The order of the pins in this array matches the order of the LEDs on the board.
int INJECTOR_PINS[MAX_NUM_INJECTORS] = { 3, 13, 12, 2, 1, 11 };

// Firing order for four-cylinder engine.
int FOUR_CYLINDER_PINS[4] = { INJECTOR_PINS[0], INJECTOR_PINS[2], INJECTOR_PINS[3], INJECTOR_PINS[1] };

// Firing order for six-cylinder engine.
int SIX_CYLINDER_PINS[6] = { INJECTOR_PINS[0], INJECTOR_PINS[4], INJECTOR_PINS[2], INJECTOR_PINS[5], INJECTOR_PINS[1], INJECTOR_PINS[3] };

// Define some values used by the panel and buttons
int LcdKey = 0;
int AdcKey = 0;

// Protothread structures
static struct pt UiThread, DriverThread;

// Used to keep track of current RPM (set by user)
int CurrentRPM = 0;

// Used to keep track of current delay for fuel injectors (calculated from RPM)
int CurrentDelay = DELAY_MAX;

// Keep track of current firing order and number of cylinders.
int* CurrentFiringOrder = FOUR_CYLINDER_PINS;
int CurrentNumCylinders = 4;

// Keep track of current RPM tenth's place;
int CurrentRpmTenthsPlace = 0;

int CurrentInjectionTime = INJECTION_TIME_MIN;

// Mode of the application.
Mode CurrentMode = DriveUI;

/* Functions */

void setup()
{
	// Initialize the threads.
	PT_INIT(&UiThread);
	PT_INIT(&DriverThread);

	// Initialize all digital pins as an outputs for the fuel injector.
	int i;
	for (i = 0; i < MAX_NUM_INJECTORS; i++)
	{		
		pinMode(INJECTOR_PINS[i], OUTPUT);
	}

	// Start the library
	LCD.begin(16, 2);
	LCD.setCursor(0, 0);

    PrintMode();

	CalculateDelayAndRpmFromPot();

	// Move to the begining of the second line
	PrintConfigInfo(CurrentRPM, CurrentInjectionTime, CurrentNumCylinders);
}

void loop()
{
	ProcessUI(&UiThread);
	DriveFuelInjectorsPT(&DriverThread);
}

// Read the buttons
int ReadLCDButtons()
{
	AdcKey = analogRead(0);
									 
	if (AdcKey > 1000) return btnNONE;
	if (AdcKey < 50)   return btnRIGHT;
	if (AdcKey < 250)  return btnUP;
	if (AdcKey < 450)  return btnDOWN;
	if (AdcKey < 650)  return btnLEFT;
	if (AdcKey < 850)  return btnSELECT;

	return btnNONE;
}

int CalculateRpmFromPot(int pot, int tenthsPlace)
{
	int rpm = (int)(RPM_MIN + (RPM_MAX - RPM_MIN) * ((float)pot / 1024));
	rpm = (rpm / LARGE_RPM_INCREMENT) * LARGE_RPM_INCREMENT;
    rpm += tenthsPlace;
	return rpm;
}

int CalculateDelayFromRpm(int rpm)
{
	int delay = DELAY_MAX;
	if (rpm > 0)
	{
	    float period = 1000.0 / (rpm / 120.0);
		delay = (int)period - CurrentInjectionTime;
		if (delay < 0)
		{
			delay = DELAY_MIN;
		}
	}

	return delay;
}

int CalculatePotValue()
{
    int rawPotValue = analogRead(POT_PIN);
    return 1024 - rawPotValue;
}

void CalculateDelayAndRpmFromPot()
{
	// Read value of potentiometer.
	int potValue = CalculatePotValue();

	// Use potentiometer to set the RPM value.
	CurrentRPM = CalculateRpmFromPot(potValue, CurrentRpmTenthsPlace);

	// Recalculate burst delay based on the current RPM setting.
	CurrentDelay = CalculateDelayFromRpm(CurrentRPM);
}

void PrintToLine(char* data, int lineNumber)
{
    LCD.setCursor(0, lineNumber);

    char dataFormatted[17];
    sprintf(dataFormatted, "%-16s", data);

    LCD.print(dataFormatted);
}

void PrintConfigInfo(int rpm, int injectionTime, int numCylinders)
{	
	char rpmAndEngineString[17];
    sprintf(rpmAndEngineString, "%-4d%-4s%-2d%4d%-1s", rpm, "RPM", injectionTime, numCylinders, "C");
    sprintf(rpmAndEngineString, "%2s%-4d%4s%-2d%3s%1d", "R:", rpm, " IT:", injectionTime, " C:", numCylinders);

    PrintToLine(rpmAndEngineString, 1);
}

void PrintMode()
{
    if (DriveInjectors == CurrentMode)
    {
        PrintToLine("Running Test...", 0);
    }
    else
    {
        PrintToLine("Configure Test", 0);
    }
}

void ToggleMode()
{
	if (DriveInjectors == CurrentMode)
	{
		CurrentMode = DriveUI;
	}
	else
	{
		CurrentMode = DriveInjectors;
	}
}

void ToggleEngineType()
{
    if (4 == CurrentNumCylinders)
    {
        CurrentNumCylinders = 6;
        CurrentFiringOrder = SIX_CYLINDER_PINS;
    }
    else
    {
        CurrentNumCylinders = 4;
        CurrentFiringOrder = FOUR_CYLINDER_PINS;
    }
}

void UpdateRpmTenthsPlace(int delta)
{
    int updatedValue = CurrentRpmTenthsPlace + delta;

    if (updatedValue >= 0 && updatedValue <= 90)
    {
        CurrentRpmTenthsPlace = updatedValue;
    }
}

void UpdateInjectionTime(int delta)
{
    int updatedValue = CurrentInjectionTime + delta;

    if (updatedValue >= INJECTION_TIME_MIN && updatedValue <= INJECTION_TIME_MAX)
    {
        CurrentInjectionTime = updatedValue;
    }
}

// TODO: Separate button reading from LCD display logic. (should be separate threads)
static int ProcessUI(struct pt *pt)
{
	PT_BEGIN(pt);

	if (DriveUI == CurrentMode)
	{
		CalculateDelayAndRpmFromPot();
		PrintConfigInfo(CurrentRPM, CurrentInjectionTime, CurrentNumCylinders);
	}

	LcdKey = ReadLCDButtons();
	switch (LcdKey)
	{		
        case btnRIGHT:
		case btnLEFT:
		{
            if (DriveUI == CurrentMode)
            {
                ToggleEngineType();
                delay(500);
            }
			break;
		}
		case btnUP:
		{
            //UpdateRpmTenthsPlace(SMALL_RPM_INCREMENT);
            if (DriveUI == CurrentMode)
            {
                UpdateInjectionTime(INJECTION_TIME_INCREMENT);
                delay(500);
            }
			break;
		}
		case btnDOWN:
		{
            //UpdateRpmTenthsPlace(-SMALL_RPM_INCREMENT);
            if (DriveUI == CurrentMode)
            {
                UpdateInjectionTime(-INJECTION_TIME_INCREMENT);
                delay(500);
            }
			break;
		}
		case btnSELECT:
		{
			ToggleMode();
            PrintMode();
			
			delay(500);
			break;
		}
		case btnNONE:
		{
			break;
		}
	}

	PT_END(pt);
}

static int DriveFuelInjectorsPT(struct pt *pt)
{
	if ( (DriveInjectors == CurrentMode) && (CurrentRPM > 0))
	{
		static unsigned long timestamp = 0;
        static int i = 0;
        PT_BEGIN(pt);

		// Execute bursts of fuel injectors in proper order.
		for (i; i < CurrentNumCylinders; i++)
		{
			digitalWrite(CurrentFiringOrder[i], HIGH);
			delay(CurrentInjectionTime);
			digitalWrite(CurrentFiringOrder[i], LOW);

            PT_WAIT_UNTIL(pt, millis() - timestamp > (CurrentDelay / CurrentNumCylinders));
            timestamp = millis();
		}
        i = 0;

		PT_END(pt);
	}
}
