/*
 * tunerstudiocomm.h
 *
 * Created: 17.1.2017 15:17:54
 *  Author: jbbja
 */ 


#ifndef TUNERSTUDIOCOMM_H_
#define TUNERSTUDIOCOMM_H_

// Page definitions from .ini file
#define VE_PAGE						1
#define CONFIG_PAGE					2
#define IGN_PAGE					3
#define AFR_PAGE					5

// Data definitions (see [OutputChannels] .ini file)
#define NUMBER_OF_REAL_TIME_BYTES	37
#define REALTIME_MAP_INDEX			4				// Manifold absolute pressure (kPa)
#define REALTIME_IAT_INDEX			5				// Intake air temperature (°c + 40°c)
#define REALTIME_CLT_INDEX			6				// Coolant temperature (°c + 40°c)
#define REALTIME_TPS_ADC_INDEX		7				// Throttle position ADC value 8 bit
#define REALTIME_BAT_INDEX			8				// Battery voltage (V)
#define REALTIME_AFR_INDEX			9				// Air to fuel ratio (AFR)
#define REALTIME_RPM_INDEX			13				// Rounds per minute (RPM)
#define REALTIME_VE_INDEX			18				// Current volumetric efficiency value (after interpolation)
#define REALTIME_PW_INDEX			20				// Pulse width (ms)
#define REALTIME_IGN_INDEX			22				// Ignition advance (deg)
#define REALTIME_TPS_INDEX			23				// Throttle position sensor (0-100%)

// Data definitions (see page = 2 in .ini file)
#define CONFIG_TPS_LOW_OFFSET		44				// Throttle position sensor lower calibration index
#define CONFIG_TPS_HIGH_OFFSET		45				// Throttle position sensor high calibration index
#define CONFIG_MAP_LOW_OFFSET		46				// Manifold absolute pressure lower calibration index
#define CONFIG_MAP_HIGH_OFFSET_FIRST_BYTE		47	// Manifold absolute pressure high calibration index
#define CONFIG_MAP_HIGH_OFFSET_SECOND_BYTE		48	// Manifold absolute pressure high calibration index

// Data length of IAT, CLT and AFR calibration vectors in bytes
#define DATA_LENGTH					LUT_SIZE // Look up tables, defined in sensors.h
#define CONFIG_CLT					0
#define CONFIG_IAT					1
#define CONFIG_AFR					2



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
uint8_t TestFlag;
int16_t TestBuffer[DATA_LENGTH];


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
void tunerstudio_write_data(uint16_t data);
// Helper function to write data to 3d table
void tunerstudio_write_to_table(uint8_t data, uint8_t table[THREE_D_TABLE_SIZE][THREE_D_TABLE_SIZE], uint8_t xbin[THREE_D_TABLE_SIZE], uint8_t ybin[THREE_D_TABLE_SIZE]);
// Burn data of current page to eeprom
void tunerstudio_burn_page_eeprom(void);
// Helper function to write changed data to eeprom
void tunerstudio_burn_table_eeporm(uint16_t EepromIndex, uint8_t table[THREE_D_TABLE_SIZE][THREE_D_TABLE_SIZE], uint8_t xbin[THREE_D_TABLE_SIZE], uint8_t ybin[THREE_D_TABLE_SIZE]);
// Helper function
void tunerstudio_burn_value_if_changed(uint32_t TempValue, uint32_t EepromIndex);
// Burn engine configuration to eeprom
void tunerstudio_burn_config(void);
// Send real time data to tunerstudio when character "A" is received
void tunerstudio_send_real_time_data(void);
// Update calibration vectors for sensors like IAT, CLT and AFR. REMEMBER TO RESTART AFTER CHANGING THESE VALUES
void tunerstudio_update_calib_vect(void);
void tunerstudio_update_calib_vect_helper(uint8_t NrOfBytes, uint32_t EepromIndex);
// Global debug function to send parameters to terminal when 'D' is transferred
void tunerstudio_debug_global_function(void);
#endif /* TUNERSTUDIOCOMM_H_ */