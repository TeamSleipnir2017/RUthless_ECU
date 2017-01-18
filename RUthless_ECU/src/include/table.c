/*
 * table.c
 *
 * Created: 18.1.2017 17:56:55
 *  Author: Rikardur
 */ 

#include "table.h"

// Initialization function for arrays
// TODO: READ FROM EEPROM !!!
void table_init(void)
{
	// Initialize VE array
	for (uint8_t i = 0; i < VE_TABLE_SIZE; i++)
	{
		for (uint8_t j = 0; j < VE_TABLE_SIZE; j++)
		{
			VeTable[i][j] = 255; // READ EEPROM !!
		}
		RpmBins[i] = 255; // READ EEPROM !!
		MapBins[i] = 255; // READ EEPROM !!
	}
		
}
