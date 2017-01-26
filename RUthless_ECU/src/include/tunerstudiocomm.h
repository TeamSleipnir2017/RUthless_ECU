/*
 * tunerstudiocomm.h
 *
 * Created: 17.1.2017 15:17:54
 *  Author: jbbja
 */ 


#ifndef TUNERSTUDIOCOMM_H_
#define TUNERSTUDIOCOMM_H_

// Page definitions from .ini file
#define VE_PAGE 1
#define IGN_PAGE 3
#define AFR_PAGE 5

// Data definitions (see [OutputChannels] .ini file)
#define NUMBER_OF_REAL_TIME_BYTES 37
#define REALTIME_MAP_INDEX 4				// Manifold absolute pressure (kPa)
#define REALTIME_IAT_INDEX 5				// Intake air temperature (°c + 40°c)
#define REALTIME_CLT_INDEX 6				// Coolant temperature (°c + 40°c)
#define REALTIME_BAT_INDEX 8				// Battery voltage (V)
#define REALTIME_AFR_INDEX 9				// Air to fuel ratio (AFR)
#define REALTIME_RPM_INDEX 13				// Rounds per minute (RPM)
#define REALTIME_VE_INDEX 18				// Current volumetric efficiency value (after interpolation)
#define REALTIME_PW_INDEX 20				// Pulse width (ms)
#define REALTIME_IGN_INDEX 22				// Ignition advance (deg)
#define REALTIME_TPS_INDEX 23				// Throttle position sensor (0-100)

#include "table.h" // Always compile error if removed not sure why JBB 20.1.17
#include "global.h"
/************************************************************************/
/* This is a communication files using Megasquirt serial protocol for 
tuner studio.                                                                     */
/************************************************************************/


// Current page variable (To send the correct page when character "V" arrives)
uint8_t CurrPage;
uint8_t NewPageFlag;
uint8_t WriteFlag;
uint8_t Offset1;
uint8_t Offset2;
uint8_t	OffsetFlag;


// Initialization function
void tunerstudio_init(void);

// Function to decide what to do when an character is received by UART
void tunerstudio_command(uint8_t character);

// Function to send page according to the .ini file (CurrPage is used)
void tunerstudio_send_page(void);

// Function to send a 16x16 table to tunerstudio
void tunerstudio_send_3d_table(uint8_t table[THREE_D_TABLE_SIZE][THREE_D_TABLE_SIZE], uint8_t xbin[THREE_D_TABLE_SIZE], uint8_t ybin[THREE_D_TABLE_SIZE]);
// Function to send dummy bytes to tunerstudio
void tunerstudio_send_dummy_data(uint16_t NumberOfBytes, uint8_t dummy);
// Function to receive data and update array
void tunerstudio_write_data(uint8_t data);
// Helper function to write data to 3d table
void tunerstudio_write_to_table(uint8_t data, uint8_t table[THREE_D_TABLE_SIZE][THREE_D_TABLE_SIZE], uint8_t xbin[THREE_D_TABLE_SIZE], uint8_t ybin[THREE_D_TABLE_SIZE]);
// Burn data of current page to eeprom
void tunerstudio_burn_page_eeprom(void);
// Helper function to write changed data to eeprom
void tunerstudio_burn_table_eeporm(uint16_t EepromIndex, uint8_t table[THREE_D_TABLE_SIZE][THREE_D_TABLE_SIZE], uint8_t xbin[THREE_D_TABLE_SIZE], uint8_t ybin[THREE_D_TABLE_SIZE]);
// Send real time data to tunerstudio when character "A" is received
void tunerstudio_send_real_time_data(void);
#endif /* TUNERSTUDIOCOMM_H_ */