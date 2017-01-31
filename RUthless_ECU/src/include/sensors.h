/*
 * sensors.h
 *
 * Created: 26.1.2017 20:45:10
 *  Author: jbbja
 */ 


#ifndef SENSORS_H_
#define SENSORS_H_

#define LUT_SIZE 1024

#include "global.h"
#include <asf.h>	// Compile error if this is not included here, not sure why 30.1.17 JBB

// Look up tables for Coolant temp, intake air temp, air to fuel ratio
volatile uint8_t CltAdc_LUT[LUT_SIZE]; 
volatile uint8_t IatAdc_LUT[LUT_SIZE];
volatile uint8_t AfrAdc_LUT[LUT_SIZE];

// Read from EEPROM
void sensors_init(void);

// Read values from ADC and convert them to real units
void sensors_read_adc(void);

#endif /* SENSORS_H_ */