/*
 * tunerstudiocomm.h
 *
 * Created: 17.1.2017 15:17:54
 *  Author: jbbja
 */ 


#ifndef TUNERSTUDIOCOMM_H_
#define TUNERSTUDIOCOMM_H_

#include "global.h"

// Page definitions from .ini file
enum PAGE {PAGE_VE = 1, PAGE_CONFIG2, PAGE_IGN, PAGE_CONFIG4, PAGE_AFR, PAGE_CONFIG6, PAGE_CONFIG7, PAGE_CONFIG8, PAGE_CONFIG9};
uint8_t CurrPage;

// Data length of IAT, CLT and AFR calibration vectors in bytes (see sensors.h), indexes below
#define CONFIG_CLT					0
#define CONFIG_IAT					1
#define CONFIG_AFR					2


/************************************************************************/
/* This is a communication files using Megasquirt serial protocol for 
tuner studio.                                                                     */
/************************************************************************/

// Current page variables (To send/write/burn the correct page when character arrives)
uint8_t NewPageFlag;
uint8_t WriteFlag;
uint8_t Offset1;
uint8_t Offset2;
uint8_t	OffsetFlag;

// Initialization function
void tunerstudio_init(void);

// Function to decide what to do when an character is received by UART
void tunerstudio_command(uint8_t character);
// Function to point to current struct and return true for table struct and false for config struct
uint8_t tunerstudio_pick_page (uint8_t *Current);
// Function to send page according to the .ini file (CurrPage is used)
void tunerstudio_send_page(void);

// Function to send a 16x16 table to tunerstudio
void tunerstudio_send_Table3D(struct Table3D *Current);
// Function to send configuration data to tunerstudio
void tunerstudio_send_struct(uint8_t *ConfigStructPointer, uint16_t ConfigLen);
// Function to receive data and update array
void tunerstudio_write_data(uint16_t data);
// Helper function to write data to 3d table
void tunerstudio_write_Table3D(struct Table3D *Current, uint8_t data);
// Write data to configuration structs
void tunerstudio_write_struct(uint8_t *ConfigStructPointer, uint8_t data);
// Burn data of current page to eeprom
void tunerstudio_burn_page_eeprom(void);
// Helper function to write changed data to eeprom
void tunerstudio_burn_Table3D(struct Table3D *Current, uint16_t EepromIndex);
// Helper function
void tunerstudio_burn_value_if_changed(uint32_t TempValue, uint16_t EepromIndex);
// Burn engine configuration to eeprom
void tunerstudio_burn_struct(uint8_t *ConfigStructPointer, uint16_t ConfigLen, uint16_t EepromIndex);
// Send real time data to tunerstudio when character "A" is received
void tunerstudio_send_real_time_data(void);
// Update calibration vectors for sensors like IAT, CLT and AFR. REMEMBER TO RESTART AFTER CHANGING THESE VALUES
void tunerstudio_update_calib_vect(void);
void tunerstudio_update_calib_vect_helper(uint8_t NrOfBytes, uint16_t EepromIndex);
// Global debug function to send parameters to terminal when 'D' is transferred
void tunerstudio_debug_global_function(void);
#endif /* TUNERSTUDIOCOMM_H_ */