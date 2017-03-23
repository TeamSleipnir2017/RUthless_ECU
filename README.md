# RUthless_ECU
Reykjavik University Engine Control Unit

## Introduction
```
To begin with, this is an ongoing project which is being updated regularly. 
Please notice that the hardware supports features that might not yet been implemented.
This controller is designed and built for a Formula Student car running an Yamaha R6 engine.  

RUthless_ECU is a general engine control unit, the hardware provides the following functions:
   - 9 High impedance injector outputs
   - 9 Transistor controlled ignition outputs
   - 1 Primary trigger input for variable reluctance sensor
   - 1 Secondary trigger input for hall effect sensor
   - 1 Speed trigger input for hall effect sensor
   - 2 Digital inputs
   - 11 Analog inputs
   - Onboard map sensor (MPX4115 can be changed)
   
The econoseal 36-pin connector provides the following pins:
   - 1: Analog auxilary input 11
   - 2: Analog coolant temperature input
   - 3: Analog intake air temperature input
   - 4: Analog external manifold absolute pressure sensor
   - 5: Analog barometric sensor
   - 6: Analog throttle position sensor 2
   - 7: Analog throttle position sensor 1
   - 8: Analog auxilary input 10
   - 9: Unused
   - 10: Unused
   - 11: Unused
   - 12: Ground
   - 13: Analog auxilary input 9
   - 14: Launch control button input
   - 15: Speed sensor input
   - 16: Ignition coil 5 output
   - 17: Ignition coil 7 output
   - 18: Ignition coil 8 output
   - 19: Ignition coil 4 output
   - 20: Ignition coil 6 output
   - 21: Ignition coil 2 output
   - 22: Ignition coil auxilary output
   - 23: Ignition coil 3 output
   - 24: Ignition coil 1 output
   - 25: Ground
   - 26: Unused
   - 27: Unused
   - 28: Injector 4 output
   - 29: Injector 2 output
   - 30: Injector 1 output
   - 31: Injector 3 output
   - 32: Injector 5 output
   - 33: Injector 7 output
   - 34: Injector auxilary output
   - 35: Injector 8 output
   - 36: Injector 6 output
The econoseal 18-pin connector provides the following pins:
   - 1: Ground
   - 2: Unused
   - 3: Unused
   - 4: Unused
   - 5: Unused
   - 6: Unused
   - 7: Sensor output voltage 5V
   - 8: Sensor output voltage 3.3V
   - 9: Unused
   - 10: Unused
   - 11: Camshaft signal input 
   - 12: Crankshaft signal input +
   - 13: Input voltage 12V
   - 14: Analog wideband sensor input
   - 15: Neutral switch input
   - 16: Reverse gear switch input
   - 17: Unused
   - 18: Crankshaft signal input -
   
There are onboard header outputs for I2C, SPI and Serial (USART). 
This is intended for remote communication or datalogging.

The software is written in C using Atmel Software Framework \url{http://www.atmel.com/tools/avrsoftwareframework.aspx}.
To tune the engine is Tunerstudio used, with modified configuration file from Speeduino \url{http://speeduino.com/wiki/index.php/Speeduino}.
```

## Important files
```
RUthless_ECU.ini
EEPROM_Management.xlsx
RUthless_ECU
├── RUthless_ECU.cproj
└── src
    ├── asf.h
    ├── main.c
    └── include
        ├── ADC.c
        ├── ADC.h
        ├── decoders.c
        ├── decoders.h
        .
        .
        .
        ├── uart.c
        └── uart.h
```

