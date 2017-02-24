/*
 * eeprom.c
 *
 * Created: 21.1.2017 14:14:22
 *  Author: jbbja
 */ 

 #include "eeprom.h"

 // Initialization function
 void eeprom_init(void)
 {
	pmc_enable_periph_clk(ID_TWI1);				// Enable peripheral clock
	#define TWI_DATA_PIN PIO_PB12A_TWD1			// Define TWI data pin
	#define TWI_CLOCK_PIN PIO_PB13A_TWCK1		// Define TWI clock pin
	#define TWI_PERIPHERAL PIOB					// Define TWI peripheral (either A(TWI0) or B(TWI1))
	TWI_PERIPHERAL->PIO_IDR = TWI_DATA_PIN;		// Interrupt disable register
	TWI_PERIPHERAL->PIO_IDR = TWI_CLOCK_PIN;	
	TWI_PERIPHERAL->PIO_PDR = TWI_DATA_PIN;		// Disable input/output which enables peripheral mode
	TWI_PERIPHERAL->PIO_PDR = TWI_CLOCK_PIN;
	TWI_PERIPHERAL->PIO_ABSR &= ~TWI_DATA_PIN;	// Enable Peripheral A (TWI)
	TWI_PERIPHERAL->PIO_ABSR &= ~TWI_CLOCK_PIN;
	TWI_PERIPHERAL->PIO_PUER = TWI_DATA_PIN;
	TWI_PERIPHERAL->PIO_PUER = TWI_CLOCK_PIN;
	// Initialize TWI 
	twi_options_t opt;
	opt.master_clk = sysclk_get_cpu_hz(); opt.speed = 400000; // 400KHz clock frequency
	twi_master_init(TWI1, &opt);
 }

// More advanced function to read from the EEPROM
// It basicly checks for TWI comm. success and retries until it enables a Fault
uint8_t eeprom_read_byte(uint16_t address)
{
	uint8_t result = 0;
	// Try to read specific number of times otherwise FAULT
	for (uint8_t i = 0; i < TWI_NUMBER_OF_TRIES; i++)
		if (!(at24cxx_read_byte(address, &result) & AT24C_READ_FAIL))
			return result;

	// Let know that there is a fault on the Two wire interface
	engine_config.TwiFault = TRUE;
	uart_print_string("TwiFault: "); uart_print_int(engine_config.TwiFault); uart_new_line();
	return 1; // 1 is the safest number regarding IGN, VE and AFR map
}

uint16_t eeprom_read_int(uint16_t address)
{
	uint16_t FirstByte = eeprom_read_byte(address);
	uint16_t SecondByte = eeprom_read_byte(address + 1);
	return (FirstByte | (SecondByte << 8));
}

uint16_t eeprom_look_up_index(uint8_t Page)
{
	switch(Page)
	{
		case PAGE_VE: return EEPROM_VE_INDEX;
		case PAGE_AFR: return EEPROM_AFR_INDEX;
		case PAGE_IGN: return EEPROM_IGN_INDEX;
		case PAGE_CONFIG2: return EEPROM_CONFIG2_INDEX;
		case PAGE_CONFIG4: return EEPROM_CONFIG4_INDEX;
		case PAGE_CONFIG6: return EEPROM_CONFIG6_INDEX;
		case PAGE_CONFIG7: return EEPROM_CONFIG7_INDEX;
		case PAGE_CONFIG8: return EEPROM_CONFIG8_INDEX;
		case PAGE_CONFIG9: return EEPROM_CONFIG9_INDEX;
	}
}