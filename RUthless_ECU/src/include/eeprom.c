/*
 * eeprom.c
 *
 * Created: 21.1.2017 14:14:22
 *  Author: jbbja
 */ 

 #include "eeprom.h"

 void eeprom_init(void)
 {
	EepromReadAddress = 0b10100001;
	EepromWriteAddress = 0b10100000;
	at24cxx_reset();
 }