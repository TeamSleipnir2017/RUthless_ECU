/*
 * table.c
 *
 * Created: 18.1.2017 17:56:55
 *  Author: Rikardur
 */ 

#include "table.h"

// Initialization function for arrays
void table_init(void)
{
	// Initialize Volumetric efficiency array
	table_init_table(EEPROM_VE_INDEX, &VeTable, &VeRpmBins, &VeMapBins);
	// Initialize Air to fuel ratio array
	table_init_table(EEPROM_AFR_INDEX, &AfrTable, &AfrRpmBins, &AfrMapBins);
	// Initialize Ignition array
	table_init_table(EEPROM_IGN_INDEX, &IgnTable, &IgnRpmBins, &IgnMapBins);
}

void table_init_table(uint16_t Index, uint8_t Table[THREE_D_TABLE_SIZE][THREE_D_TABLE_SIZE], uint8_t RpmBins[THREE_D_TABLE_SIZE], uint8_t MapBins[THREE_D_TABLE_SIZE])
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
