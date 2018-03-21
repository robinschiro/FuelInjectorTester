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
#define NUM_INJECTORS 6
#define POT_PIN 5
#define RPM_INCREMENT 600
#define RPM_MAX 8000
#define RPM_MIN 0
#define INJECTOR_BURST_TIME 5
#define DELAY_MIN 10
#define DELAY_MAX 15000

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
int INJECTOR_PINS[NUM_INJECTORS] = { 1, 2, 3, 11, 12, 13 };

// Define some values used by the panel and buttons
int LcdKey = 0;
int AdcKey = 0;

// Protothread structures
static struct pt UiThread, DriverThread;

// Used to keep track of current RPM (set by user)
int CurrentRPM = 0;

// Used to keep track of current delay for fuel injectors (calculated from RPM)
int CurrentDelay = DELAY_MAX;

// Keep track of potentiometer value
int PotValue = 0;

// Mode of the application.
Mode CurrentMode = DriveInjectors;

/* Functions */


void setup()
{
	// Initialize the threads.
	PT_INIT(&UiThread);
	PT_INIT(&DriverThread);

	// Initialize all digital pins as an outputs for the fuel injector.
	int i;
	for (i = 0; i < NUM_INJECTORS; i++)
	{		
		pinMode(INJECTOR_PINS[i], OUTPUT);
	}

	// Start the library
	LCD.begin(16, 2);
	LCD.setCursor(0, 0);

	// Print a simple message
	LCD.print("Select RPM");

	CalculateDelayAndRpmFromPot();

	// Move to the begining of the second line
	LCD.setCursor(0, 1);
	LCD.print(CurrentRPM);
}

void loop()
{
	ProcessUI(&UiThread);
	DriveFuelInjectorsPT(&DriverThread);
	//DriveFuelInjectorsNonPT();
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

int CalculateRpmFromPot(int pot)
{
	int rpm = (int)(RPM_MIN + (RPM_MAX - RPM_MIN) * ((float)pot / 1024));
	rpm = (rpm / 100) * 100;
	return rpm;
}

int CalculateDelayFromRpm(int rpm)
{
	int delay = DELAY_MAX;
	if (rpm > 0)
	{
	    float period = 1000.0 / (rpm / 120.0);
		delay = (int)period - INJECTOR_BURST_TIME;
		if (delay < 0)
		{
			delay = DELAY_MIN;
		}
	}

	return delay;
}

void CalculateDelayAndRpmFromPot()
{
	// Read value of potentiometer.
	PotValue = analogRead(POT_PIN);

	// Use potentiometer to set the RPM value.
	CurrentRPM = CalculateRpmFromPot(PotValue);

	// Recalculate burst delay based on the current RPM setting.
	CurrentDelay = CalculateDelayFromRpm(CurrentRPM);
}

void PrintRPM(int rpm)
{
	// Create formatted RPM string.
	char rpmFormatted[17];
	sprintf(rpmFormatted, "%-8d", rpm);

	// Print to lcd.
	LCD.setCursor(0, 1);
	LCD.print(rpmFormatted);
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

// TODO: Separate button reading from LCD display logic. (should be separate threads)
static int ProcessUI(struct pt *pt)
{
	PT_BEGIN(pt);

	if (DriveUI == CurrentMode)
	{
		CalculateDelayAndRpmFromPot();
		PrintRPM(CurrentRPM);
	}

	LcdKey = ReadLCDButtons();
	switch (LcdKey)
	{
		case btnRIGHT:
		{
			break;
		}
		case btnLEFT:
		{
			break;
		}
		case btnUP:
		{
			//currentRPM += RPM_INCREMENT;
			delay(500);
			break;
		}
		case btnDOWN:
		{
			//currentRPM -= RPM_INCREMENT;
			delay(500);
			break;
		}
		case btnSELECT:
		{
			// Toggle mode when select is hit.
			ToggleMode();
			
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
	if ( (DriveInjectors == CurrentMode) && (CurrentDelay > INJECTOR_BURST_TIME) && (CurrentRPM > 0))
	{
		static unsigned long timestamp = 0;
		PT_BEGIN(pt);

		// Execute bursts of fuel injectors in proper order.
		int i;
		for (i = 0; i < NUM_INJECTORS; i++)
		{
			digitalWrite(INJECTOR_PINS[i], HIGH);
			delay(INJECTOR_BURST_TIME);
			digitalWrite(INJECTOR_PINS[i], LOW);
		}

		// Wait for small amount of time before bursting again.
		PT_WAIT_UNTIL(pt, millis() - timestamp > CurrentDelay);
		timestamp = millis();

		PT_END(pt);
	}
}

void DriveFuelInjectorsNonPT()
{
	if (CurrentDelay > 0)
	{
		int i;
		for ( i = 0; i < NUM_INJECTORS; i++ )
		{
			digitalWrite(INJECTOR_PINS[i], HIGH);
			delay(INJECTOR_BURST_TIME);
			digitalWrite(INJECTOR_PINS[i], LOW);
		}
		delay(CurrentDelay);
	}
}

