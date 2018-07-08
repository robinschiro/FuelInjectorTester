# FuelInjectorTester
An Arduino-based device used to control fuel injectors for testing purposes.

# Background Information

- Injector injects onces per two turns of the engine

# Compiling the Code

- Get the Arduino IDE (https://www.arduino.cc/en/Main/OldSoftwareReleases)
- Unzip all dependencies (located in the 'Dependencies' folder) into the Arduino 'libraries' folder. The Arduino 'libraries' folder is located in the following locations (platform dependent):
	- (Windows) C:\Users\{User}\Documents\Arduino\libraries
	- (Unix) ~/sketchbook/libraries
- Open the 'FuelInjectorTester.ino' file using the Arduino IDE
- Click the button with a checkmark (top-left corner) to compile the code.

# Uploading Code to Arduino



# Requirements

- Support 4, 6, or 8 injectors
- LED lights that light up with fuel injectors
- Be able to adjust the following:
    - Number of injectors (4, 6 or 8)
    - Injection Time
    - RPM
    - Length of time the test will run
- Support different modes
    - Each mode should have configuration screen and running screen
    - Modes:
        - Testing
            - Injection Time: Range from 2 - 12 ms
            - RPM: range from 0 - 8000, increments of 100
        - Cleaning
            - Injection Time should range from 5 - 20 seconds, increments of 5
            - RPM: Range from 1 - 200 (use knob), increments of 1

# Problems
- If printing to LCD screen every frame, digital output to pins other than 13 does not work.
    - Possible Solution: Only drive the fuel injectors when in DRIVE_INJECTORS mode. In this mode, the UI is not updated. After exiting this mode, the UI will become responsive again.

# Express PCB Plus Grid Layout:
Grid: 2.5 mm
Snap: 0.25 mm


# Open questions
- Does simulating the firing order really matter?
	- Downside: Requires extra logic in the code