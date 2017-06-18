/*
 * debug.c
 *
 * Created: 17.6.2017 10:44:10
 *  Author: Jón Bjarni Bjarnason
 */ 

#include "debug.h"

/************************************************************************/
/* Table 35-3. I/O Lines
Instance Signal I/O Line Peripheral
USART0 CTS0 PB26 A
USART0 RTS0 PB25 A
USART0 RXD0 PA10 A
USART0 SCK0 PA17 B
USART0 TXD0 PA11 A
USART1 CTS1 PA15 A
USART1 RTS1 PA14 A
USART1 RXD1 PA12 A
USART1 SCK1 PA16 A
USART1 TXD1 PA13 A
USART2 CTS2 PB23 A
USART2 RTS2 PB22 A
USART2 RXD2 PB21 A
USART2 SCK2 PB24 A
USART2 TXD2 PB20 A
USART3 CTS3 PF4 A
USART3 RTS3 PF5 A
USART3 RXD3 PD5 B
USART3 SCK3 PE16 B
USART3 TXD3 PD4 B                                                       */
/************************************************************************/

#define USART_BAUDRATE 115200
#define USART_CLOCKDIVISION ATSAM3X8E_MCK/(USART_BAUDRATE*16)


void debug_init_usart(debug_communication *Instance, Usart *UsartInstance, uint32_t BaudRate) // UsartInstance: USART0, USART1 ...
{
	/* Initialize USART controller */
	uint32_t ID;
	if (UsartInstance == USART0) // SAM3X8E has only 4 controllers
	{
		ID = ID_USART0;
		PIOA->PIO_IDR = PIO_PA10A_RXD0 | PIO_PA11A_TXD0;		// Interrupt disable register
		PIOA->PIO_PDR = PIO_PA10A_RXD0 | PIO_PA11A_TXD0;		// Disable input/output which enables peripheral mode
		PIOA->PIO_ABSR &= ~(PIO_PA10A_RXD0 | PIO_PA11A_TXD0);	// Enable Peripheral A (USART)
	}
	else if (UsartInstance == USART1)
	{
		ID = ID_USART1;
		PIOA->PIO_IDR = PIO_PA12A_RXD1 | PIO_PA13A_TXD1;		// Interrupt disable register
		PIOA->PIO_PDR = PIO_PA12A_RXD1 | PIO_PA13A_TXD1;		// Disable input/output which enables peripheral mode
		PIOA->PIO_ABSR &= ~(PIO_PA12A_RXD1 | PIO_PA13A_TXD1);	// Enable Peripheral A (USART)
	} 
	else if (UsartInstance == USART2)
	{
		ID = ID_USART2;
		PIOB->PIO_IDR = PIO_PB20A_TXD2 | PIO_PB21A_RXD2;		// Interrupt disable register
		PIOB->PIO_PDR = PIO_PB20A_TXD2 | PIO_PB21A_RXD2;		// Disable input/output which enables peripheral mode
		PIOB->PIO_ABSR &= ~(PIO_PB20A_TXD2 | PIO_PB21A_RXD2);	// Enable Peripheral A (USART)
	}
	else if (UsartInstance == USART3)
	{
		ID = ID_USART3;
		PIOD->PIO_IDR = PIO_PD4B_TXD3 | PIO_PD5B_RXD3;		// Interrupt disable register
		PIOD->PIO_PDR = PIO_PD4B_TXD3 | PIO_PD5B_RXD3;		// Disable input/output which enables peripheral mode
		PIOD->PIO_ABSR |= (PIO_PD4B_TXD3 | PIO_PD5B_RXD3);	// Enable Peripheral B (USART)
	}
	else { /*FAILED TO INITIALIZE*/ return; }

	pmc_enable_periph_clk(ID);
	
	const sam_usart_opt_t usart_console_settings = {
		.baudrate = BaudRate,
		.char_length = US_MR_CHRL_8_BIT,
		.parity_type = US_MR_PAR_NO,
		.stop_bits = US_MR_NBSTOP_1_BIT,
		.channel_mode = US_MR_CHMODE_NORMAL,
		.irda_filter = 0
	};
	
	usart_init_rs232(UsartInstance, &usart_console_settings, sysclk_get_cpu_hz());
	usart_enable_tx(UsartInstance);
	// usart_enable_rx(USART0);
	
	/* Initialize Debug */
	Pdc *MyInterface = usart_get_pdc_base(UsartInstance);
	debug_new_instance(Instance, MyInterface);
}

void debug_shitmix_init(debug_communication *Instance) // don't have time to make the general function 
{
	#define USART_SERIAL_BAUDRATE        115200
	#define USART_RXD0 PIO_PA10A_RXD0
	#define USART_TXD0 PIO_PA11A_TXD0
	#define USART_PERIPHERAL PIOA
	
	USART_PERIPHERAL->PIO_IDR = USART_RXD0;		// Interrupt disable register
	USART_PERIPHERAL->PIO_IDR = USART_TXD0;
	USART_PERIPHERAL->PIO_PDR = USART_RXD0;		// Disable input/output which enables peripheral mode
	USART_PERIPHERAL->PIO_PDR = USART_TXD0;
	USART_PERIPHERAL->PIO_ABSR &= ~USART_RXD0;	// Enable Peripheral A (USART)
	USART_PERIPHERAL->PIO_ABSR &= ~USART_TXD0;

	const sam_usart_opt_t usart_console_settings = {
		.baudrate = USART_SERIAL_BAUDRATE,
		.char_length = US_MR_CHRL_8_BIT,
		.parity_type = US_MR_PAR_NO,
		.stop_bits = US_MR_NBSTOP_1_BIT,
		.channel_mode = US_MR_CHMODE_NORMAL,
		.irda_filter = 0
	};

	pmc_enable_periph_clk(ID_USART0);
	usart_init_rs232(USART0, &usart_console_settings, sysclk_get_cpu_hz());
	usart_enable_rx(USART0);
	usart_enable_tx(USART0);
	//USART0->US_BRGR = USART_CLOCKDIVISION;
	USART0->US_IDR = 0xFFFF;
	
	
	volatile uint8_t send[33] = "abcdefghijklmnoprstuqw1234567890\n";
	debug_write_usart_buffer(USART0, &send, 33);

// 	
//	debug_new_instance(Instance, usart_get_pdc_base(USART0));
	
// 	Pdc *PdcInterface = usart_get_pdc_base(USART0);
// 	
// 	uint8_t send[4] = "abcd";
// 	
// 	pdc_packet_t PdcPacket = {
// 		.ul_addr = &send,
// 		.ul_size = 4
// 	};

// 	pdc_tx_init(PdcInterface, &PdcPacket, NULL);
// 	pdc_enable_transfer(PdcInterface, PERIPH_PTCR_TXTEN);
}

void debug_new_instance(debug_communication *Instance, Pdc *PdcInterface)
{
	Instance->PdcInterface = PdcInterface;
	Instance->CurrentTransferringPacket = 0;
	Instance->Packet1Counter = 0;
	Instance->Packet2Counter = 0;
}

void debug_transfer_new_message(debug_communication * Instance, uint32_t Time, char * String, uint32_t Value)
{
	uint8_t *NextPacket;
	uint16_t *NextCounter;
	if (Instance->CurrentTransferringPacket == 0)
	{
		NextPacket = &Instance->Packet2;
		NextCounter = &Instance->Packet2Counter;
// 		NextPacket[(*NextCounter)++] = 'P';NextPacket[(*NextCounter)++] = 'A';NextPacket[(*NextCounter)++] = 'C';NextPacket[(*NextCounter)++] = 'K';NextPacket[(*NextCounter)++] = '2';
	}
	else
	{
		NextPacket = &Instance->Packet1;
		NextCounter = &Instance->Packet1Counter;
// 		NextPacket[(*NextCounter)++] = 'P';NextPacket[(*NextCounter)++] = 'A';NextPacket[(*NextCounter)++] = 'C';NextPacket[(*NextCounter)++] = 'K';NextPacket[(*NextCounter)++] = '1';
	}
	
// 	NextPacket[(*NextCounter)++] = 'C';NextPacket[(*NextCounter)++] = 'N';NextPacket[(*NextCounter)++] = 'T';
// 	debug_add_int_to_char_array(NextPacket, (*NextCounter), NextCounter);
	
	if ((*NextCounter) > MAX_FILL_LENGTH)
	{
		// ADD SOMETHING TO LET KNOW
		return;
	}
	
	NextPacket[(*NextCounter)++] = '[';
	debug_add_int_to_char_array(NextPacket, Time, NextCounter);
	NextPacket[(*NextCounter)++] = ']';
	
	uint8_t j = 0;
	while (String[j] != 0 && j < MAX_STRING_LENGTH)
	{
		NextPacket[(*NextCounter)++] = String[j++];
	}
	NextPacket[(*NextCounter)++] = ' '; // space
	
	debug_add_int_to_char_array(NextPacket, Value, NextCounter);
	
	NextPacket[(*NextCounter)++] = 10; // new line
	
	if (debug_write_usart_buffer(USART0, NextPacket, (*NextCounter)))
	{
		Instance->CurrentTransferringPacket ^= 1; //Toggle to next packet
		(*NextCounter) = 0;
	}
}

void debug_add_int_to_char_array(uint8_t *array, uint32_t data, uint16_t *counter) // returns counter
{
	uint32_t div = 1000000000;		// Divider to divide data with
	uint8_t start = 0;
	uint8_t cnt = 10;
	for (int i = 1; i <= cnt; i++)
	{
		uint8_t send = data / div + 48; // calculate the Ascii for each number
		if(send != 48 || start == 1 || i == cnt)
		{
			array[(*counter)++] = send;
			start = 1;
		}
		data %= div;
		div /= 10;
	}
}

uint8_t debug_write_usart_buffer(Usart *usart, uint32_t *buffer, uint32_t size)
{
	/* Check if the first PDC bank is free*/
	if ((usart->US_TCR == 0) && (usart->US_TNCR == 0)) 
	{
		usart->US_TPR = buffer;
		usart->US_TCR = size;
		usart->US_PTCR = PERIPH_PTCR_TXTEN;
		return 1;
	}
	/* Check if the second PDC bank is free*/
// 	else if (pdc->PERIPH_TNCR == 0) 
// 	{
// 		pdc->PERIPH_TNPR = buffer;
// 		pdc->PERIPH_TNCR = size;
// 		return 1;
// 	}
	return 0;
}



void debug_add_to_packet(debug_communication * Instance, uint32_t Time, char * String, uint32_t Value, uint8_t * CurrentPacket)
{
	
}

uint8_t debug_peripheral_is_available(debug_communication * Instance)
{
	
}

void debug_send_message(debug_communication * Instance, uint8_t * CurrentPacket)
{
	
}

void debug_TEST_component()
{
	// Create instance
	volatile debug_communication myInstance;
	
	debug_init_usart(&myInstance, USART0, 115200);
	//debug_shitmix_init(&myInstance);
	
	
	debug_transfer_new_message(&myInstance, TC2->TC_CHANNEL[2].TC_CV, "TEST_1 ", 123456789);
	debug_transfer_new_message(&myInstance, TC2->TC_CHANNEL[2].TC_CV, "TEST_2 ", 987654321);
	
	for (uint16_t i = 0; i <= MAX_PACKET_LENGTH - 1; i++) // trying to fill buffer (packet)
	{
		debug_transfer_new_message(&myInstance, TC2->TC_CHANNEL[2].TC_CV, "TEST_", i);
	}
}
