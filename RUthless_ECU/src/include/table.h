/*
 * table.h
 *
 * Created: 18.1.2017 17:57:06
 *  Author: Rikardur
 */ 


#ifndef TABLE_H_
#define TABLE_H_



#include "global.h"



/************************************************************************/
/* This file consists of all the tables(arrays) to configure or tune the
ECU                                                                     */
/************************************************************************/

// Arrays
// Volumetric Efficiency
volatile uint8_t VeTable[THREE_D_TABLE_SIZE][THREE_D_TABLE_SIZE];
volatile uint8_t VeRpmBins[THREE_D_TABLE_SIZE];
volatile uint8_t VeMapBins[THREE_D_TABLE_SIZE];

// Air to fuel ratio
volatile uint8_t AfrTable[THREE_D_TABLE_SIZE][THREE_D_TABLE_SIZE];
volatile uint8_t AfrRpmBins[THREE_D_TABLE_SIZE];
volatile uint8_t AfrMapBins[THREE_D_TABLE_SIZE];

// Ignition
volatile uint8_t IgnTable[THREE_D_TABLE_SIZE][THREE_D_TABLE_SIZE];
volatile uint8_t IgnRpmBins[THREE_D_TABLE_SIZE];
volatile uint8_t IgnMapBins[THREE_D_TABLE_SIZE];

// Initialization function for arrays
// TODO: READ FROM EEPROM
void table_init(void);
// Helper function
void table_init_table(uint16_t Index, uint8_t Table[THREE_D_TABLE_SIZE][THREE_D_TABLE_SIZE], uint8_t RpmBins[THREE_D_TABLE_SIZE], uint8_t MapBins[THREE_D_TABLE_SIZE]);


#endif /* TABLE_H_ */