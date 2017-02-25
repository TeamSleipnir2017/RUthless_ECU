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
	at24cxx_read_continuous(EepromIndex, ConfigLen, ConfigStructPointer);
// 	 for (uint16_t i = 0; i < ConfigLen; i++)
// 	 {
// 		 *(ConfigStructPointer + i) = eeprom_read_byte(EepromIndex + i);
// 	 }
 }