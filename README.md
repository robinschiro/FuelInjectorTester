# FuelInjectorTester
An Arduino-based device used to control fuel injectors for testing purposes.


# Requirements

- Injector open for 3 ms at 1000 RPM
- At 6000 RPM, it can be open for 12-15 ms
- Have adjustment to have only one open at a time. for 4 injectors, 1 3 4 2 is firing order
- Support 6 injectors (option to have 6 or 4 injectors)
- LED lights that light up with fuel injector

injectors go from from 2 ms to 15 ms

injector feeds for 15ms. then it closes. It is closed for 2 turns of the engine

injector injects onces per two turns of engine

Example: engine runs at 1000 RPM

2 ms to 15 ms is always the range of time that it is open
Injector opens every two rotations of the engine



Issue with Arduino/Protothreads/Printing to LCD Every Frame:
- If printing to LCD screen every frame, digital output to pins other than 13 does not work.

Possible Solution:
    - Only drive the fuel injectors when in DRIVE_INJECTORS mode. In this mode, the UI is not updated. After exiting this mode, the UI will become responsive again.
    
Plan For Soldering Prototype Board:
De-solder the "white" LED, try to find actual white one.
    - If can't find, then at least swap position with the Red LED that is currently in the 6th position.
- Solder in all cables that are underneath the board
    - Verify connectivity
- Solder in all cables on top layer
    - Verify connectivity
- Solder in resistors
    - Will need to find 2 additional 100 OHM resistors in lab
- Solder in transitors
- Solder in potentiometer
- Create solder bridges (excluding fuel injector cable connection points)
    



# Express PCB Plus Grid Layout:
Grid: 2.5 mm
Snap: 0.25 mm