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
	// EEPROM memory index
	uint16_t EepromIndex = 0;
	// Initialize VE array
	table_init_table(&EepromIndex, &VeTable, &VeRpmBins, &VeMapBins);
	// Initialize AFR array
	table_init_table(&EepromIndex, &AfrTable, &AfrRpmBins, &AfrMapBins);
	// Initialize AFR array
	table_init_table(&EepromIndex, &IgnTable, &IgnRpmBins, &IgnMapBins);

	
// 	for (uint8_t i = 0; i < THREE_D_TABLE_SIZE; i++)
// 	{
// 		for (uint8_t j = 0; j < THREE_D_TABLE_SIZE; j++)
// 		{
// 			VeTable[i][j] = 255; // READ EEPROM !!
// 		}
// 		VeRpmBins[i] = 255; // READ EEPROM !!
// 		VeMapBins[i] = 255; // READ EEPROM !!
// 	}	
}

void table_init_table(uint16_t * Index, uint8_t Table[THREE_D_TABLE_SIZE][THREE_D_TABLE_SIZE], uint8_t RpmBins[THREE_D_TABLE_SIZE], uint8_t MapBins[THREE_D_TABLE_SIZE])
{
	// Read from EEPROM the table (returns 1 if EEPROM is not connected)
	for (uint8_t i = 0; i < THREE_D_TABLE_SIZE; i++)
		for (uint8_t j = 0; j < THREE_D_TABLE_SIZE; j++)
			Table[i][j] = eeprom_read_byte(Index++);  
	
	// Read from EEPROM the RPM values (returns 1 if EEPROM is not connected)
	for (uint8_t i = 0; i < THREE_D_TABLE_SIZE; i++)
		RpmBins[i] = eeprom_read_byte(Index++); 
	
	// Read from EEPROM the MAP values (returns 1 if EEPROM is not connected)
	for (uint8_t i = 0; i < THREE_D_TABLE_SIZE; i++)
		MapBins[i] = eeprom_read_byte(Index++); 
}
