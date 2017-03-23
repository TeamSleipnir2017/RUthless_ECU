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

The software is written in C using Atmel Software Framework <a href="http://www.atmel.com/tools/avrsoftwareframework.aspx"></a>.
To tune the engine is Tunerstudio used, with modified configuration file from Speeduino <a href="http://speeduino.com/wiki/index.php/Speeduino"></a>.
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
## Overview of each file
```
RUthless_ECU.ini - Includes configuration parameters to connect the controller to Tunerstudio.
EEPROM_Management.xlsx - List of how the EEPROM is organized.
RUthless_ECU.cproj - Atmel Studio project file.
asf.h - Includes for Atmel Software Framework.
main.c - Main loop, uses initializing and calculation function from other files.

ADC.c & .h - Analog to digital converter interface, includes the following functions:
   - adc_initialize: Initializes the controller with 10 bit resolution
   - adc_turn_on_multiple_channels: Turns on vector of adc channels and turns on interrupts
   - ADC_Handler: Interrupt handler, stores the values to a local array "AdcData"
   
decoders.c & .h - Decoder for variable reluctance sensor, includes the following functions:
   - decoders_crank_primary: Checks for missing tooth and calculates ignition and injector timing.
   
eeprom.c & .h - Interface with at24c256 read,write and initialize, includes the following functions:
   - eeprom_init: Initializes I2C interface with at24c256
   - eeprom_read_byte: Reads a single byte from a specific address
   - eeprom_read_int: Reads two bytes from a specific address
   - eeprom_look_up_index: Returns the address for configuration pages from storage.h (see also EEPROM_Management.xlsx)
   
fuelcalc.c & .h - Calculation function for injector pulsewidth, includes the following functions:
   - fuelcalc_pulsewidth: Calculate required pulsewidth from PV=nRT including enrichments
   - fuelcalc_GammaEnrich: Calculate enrichment from coolant temp., cranking, afterstart and throttle position acceleration.
   
global.c & .h - File including global definitions and instances of variables, includes the following functions:
   - global_init: Initializes the memory from EEPROM
   - cylinder_init: Initialize real time struct of ignition and injector timings

interrupts.c & .h - Basic initialization for interupts and handlers, includes the following functions:
   - interrupts_enable_interrupt_vector: Initializes Nested Vector Interrupt Controller (NVIC) using peripheral and priority
   - interrupts_enable_pio: Enable input interrupt for a specific pin, includes these modes pin change, rising and falling edge
   - PIOA_Handler: Handler for primary, secondary and speed trigger interrupt   

   
```

