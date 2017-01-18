/*
 * tunerstudiocomm.h
 *
 * Created: 17.1.2017 15:17:54
 *  Author: jbbja
 */ 


#ifndef TUNERSTUDIOCOMM_H_
#define TUNERSTUDIOCOMM_H_

#include "global.h"
/************************************************************************/
/* This is a communication files using Megasquirt serial protocol for 
tuner studio.                                                                     */
/************************************************************************/


// Page definitions from .ini file
#define VE_PAGE 1

// Current page variable (To send the correct page when character "V" arrives)
uint8_t CurrPage;

// Initialization function
void tunerstudio_init(void);

// Function to decide what to do when an character is received by UART
void tunerstudio_command(uint8_t character);

// Function to send page according to the .ini file (CurrPage is used)
void send_page(void);

// Function to send a 16x16 table to tunerstudio
//void send_3d_table(uint8_t table[THREE_D_TABLE_SIZE][THREE_D_TABLE_SIZE], uint8_t xbin[THREE_D_TABLE_SIZE], uint8_t xbin[THREE_D_TABLE_SIZE]);

#endif /* TUNERSTUDIOCOMM_H_ */