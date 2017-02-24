/*
 * storage.c
 *
 * Created: 22.2.2017 17:39:09
 *  Author: jbbja
 */ 

 #include "storage.h"


 // Read the EEPROM and store the according byte in ascending order
 void global_struct_read_eeprom_init(uint8_t *ConfigStructPointer, uint16_t ConfigLen, uint16_t EepromIndex)
 {
	 //uint32_t at24cxx_read_continuous(uint32_t u32_start_address,
	 //uint16_t u16_length, uint8_t *p_rd_buffer)
	 for (uint16_t i = 0; i < ConfigLen; i++)
	 {
		 *(ConfigStructPointer + i) = eeprom_read_byte(EepromIndex + i);
	 }
 }