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
void tunerstudio_write_to_table(uint8_t table[THREE_D_TABLE_SIZE][THREE_D_TABLE_SIZE], uint8_t xbin[THREE_D_TABLE_SIZE], uint8_t ybin[THREE_D_TABLE_SIZE], uint8_t data);

#endif /* TUNERSTUDIOCOMM_H_ */