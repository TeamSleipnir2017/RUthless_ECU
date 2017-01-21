/*
 * eeprom.h
 *
 * Created: 21.1.2017 14:14:36
 *  Author: jbbja
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_
/************************************************************************/
/* Arduino Due core has the possibility to include an EEPROM on the PCB.
More info here: http://www.inhaos.com/uploadfile/otherpic/DOC-DUE-CORE-V01-20160611.pdf
The EEPROM used is AT24C256C-SSHL-T, and the addresses are as follows
 A0=A1=A2 = GND    
 Devices address is therefore according to: http://www.atmel.com/images/atmel-8568-seeprom-at24c256c-datasheet.pdf
 1 0 1 0 A2 A1 A0 R/(NOT)W
 MSB				   LSB
 So it would be: 1010 000x
 Read: x = 1
 Write: x = 0                                                                 */
/************************************************************************/

#define BOARD_AT24C_ADDRESS 0b10100000
#define BOARD_AT24C_TWI_INSTANCE TWI0
#define BOARD_CLK_TWI_EEPROM 18 // PIO_PA18A_TWCK0
#define BOARD_CLK_TWI_MUX_EEPROM PIOA
#define MAX_EEPROM_BYTES 32768

#include "global.h"

uint8_t EepromReadAddress, EepromWriteAddress;

// Initialization function
void eeprom_init(void);

// Use Atmel software framework functions:
// at24cxx_read_byte (uint32_t u32_address, uint8_t *p_rd_byte)
// at24cxx_read_continuous (uint32_t u32_start_address, uint16_t u16_length, uint8_t *p_rd_buffer)
// at24cxx_read_page (uint32_t u32_page_address, uint32_t u32_page_size, uint8_t *p_rd_buffer)
// at24cxx_reset (void)
// at24cxx_write_byte (uint32_t u32_address, uint8_t u8_value)



#endif /* EEPROM_H_ */