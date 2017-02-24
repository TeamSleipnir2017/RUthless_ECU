/*
 * table.c
 *
 * Created: 18.1.2017 17:56:55
 *  Author: Rikardur
 */ 

#include "tables.h"

void tables_Table3D_alloc_mem(struct Table3D *Current, uint8_t Xsize, uint8_t Ysize)
{
// LSD, lína svo dálkur (line then column)
	Current->Table = (uint8_t **)malloc(Ysize * sizeof(uint8_t*));
	for (uint8_t i = 0; i < Ysize; i++)
		Current->Table[i] = (uint8_t *)malloc(Xsize * sizeof(uint8_t));
	Current->Xbin = (uint8_t *)malloc(Xsize * sizeof(uint8_t));
	Current->Ybin = (uint8_t *)malloc(Ysize * sizeof(uint8_t));
	Current->Xsize = Xsize;
	Current->Ysize = Ysize;
	Current->Bytes = (uint16_t) Xsize * Ysize + Xsize + Ysize;
}

void tables_Table3D_read_eeprom(struct Table3D *Current, uint16_t EepromIndex)
{
	for (uint8_t i = 0; i < Current->Ysize; i++)
	{
		global_struct_read_eeprom_init(Current->Table[i], Current->Xsize, EepromIndex);
		EepromIndex += Current->Xsize;
	} 
	global_struct_read_eeprom_init(Current->Xbin, Current->Xsize, EepromIndex);
	EepromIndex += Current->Xsize;
	global_struct_read_eeprom_init(Current->Ybin, Current->Ysize, EepromIndex);
}

void tables_Table3D_init(struct Table3D *Current, uint8_t Xsize, uint8_t Ysize, uint16_t EepromIndex)
{
	tables_Table3D_alloc_mem(Current, Xsize, Ysize);
	tables_Table3D_read_eeprom(Current, EepromIndex);
}

// Initialization function for arrays
void table_init(void)
{
	tables_Table3D_init(&VE, THREE_D_TABLE_SIZE, THREE_D_TABLE_SIZE, EEPROM_VE_INDEX);
	tables_Table3D_init(&AFR, THREE_D_TABLE_SIZE, THREE_D_TABLE_SIZE, EEPROM_AFR_INDEX);
	tables_Table3D_init(&IGN, THREE_D_TABLE_SIZE, THREE_D_TABLE_SIZE, EEPROM_IGN_INDEX);
}
