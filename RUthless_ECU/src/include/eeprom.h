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

// The address of the chip is stored in at24cxx.h
// 	#define BOARD_AT24C_TWI_INSTANCE TWI1
// 	#define BOARD_AT24C_ADDRESS 0x50
// 	#define BOARD_CLK_TWI_EEPROM 13
// 	#define BOARD_CLK_TWI_MUX_EEPROM PIOB
#define BOARD_CLK_HZ			100000			// TWI Bus Clock 400kHz 
#define MAX_EEPROM_BYTES		32768			// Still not used 30.1.17 JBB
#define TWI_NUMBER_OF_TRIES		10				// Constant of number of tries to read from the EEPROM

// EEPROM address structure see excel document EEPROM_Management.xlsx
#define EEPROM_VE_INDEX					0
#define EEPROM_AFR_INDEX				288
#define EEPROM_IGN_INDEX				576
#define EEPROM_CLT_ADC_INDEX			864
#define EEPROM_IAT_ADC_INDEX			1888
#define EEPROM_AFR_ADC_INDEX			2912
#define EEPROM_TPS_LOW_INDEX			3936
#define EEPROM_TPS_HIGH_INDEX			3937
#define EEPROM_MAP_LOW_INDEX			3938
#define EEPROM_MAP_HIGH_INDEX			3939
#define EEPROM_INJ_OPEN_TIME_INDEX		3941



#include "global.h"

// Initialization function
void eeprom_init(void);
// More advanced function to read from the EEPROM
// It basicly checks for TWI comm. success and retries until it enables a Fault
uint8_t eeprom_read_byte(uint16_t address);
uint16_t eeprom_read_int(uint16_t address);



// Use Atmel software framework functions:
// at24cxx_read_byte (uint32_t u32_address, uint8_t *p_rd_byte)
// at24cxx_read_continuous (uint32_t u32_start_address, uint16_t u16_length, uint8_t *p_rd_buffer)
// at24cxx_read_page (uint32_t u32_page_address, uint32_t u32_page_size, uint8_t *p_rd_buffer)
// at24cxx_reset (void)
// at24cxx_write_byte (uint32_t u32_address, uint8_t u8_value)



#endif /* EEPROM_H_ */