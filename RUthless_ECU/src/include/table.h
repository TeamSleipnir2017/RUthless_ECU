/*
 * table.h
 *
 * Created: 18.1.2017 17:57:06
 *  Author: Rikardur
 */ 


#ifndef TABLE_H_
#define TABLE_H_

// Array size definitions
#define THREE_D_TABLE_SIZE 16

#include "global.h"

/************************************************************************/
/* This file consists of all the tables(arrays) to configure or tune the
ECU                                                                     */
/************************************************************************/



// Arrays
// Volumetric Efficiency
uint8_t VeTable[THREE_D_TABLE_SIZE][THREE_D_TABLE_SIZE];
uint8_t RpmBins[THREE_D_TABLE_SIZE];
uint8_t MapBins[THREE_D_TABLE_SIZE];

// Initialization function for arrays
// TODO: READ FROM EEPROM
void table_init(void);


#endif /* TABLE_H_ */