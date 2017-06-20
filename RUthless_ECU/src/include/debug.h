/*
 * debug.h
 *
 * Created: 17.6.2017 10:44:31
 *  Author: Jón Bjarni Bjarnason
 */ 


#ifndef DEBUG_H_
#define DEBUG_H_

#include "global.h"

/************************************************************************/
/* The purpose of this file is to make an efficient debugging way to 
make debugging a part of the code without taking to much processor power.
This will be implemented by PDC (Peripheral DMA Controller), and should 
therefore be able to put the data to serial                             */
/************************************************************************/
#define MAX_PACKET_LENGTH 20000 // 16 bit uint
#define MAX_FILL_LENGTH 19900
#define MAX_STRING_LENGTH 30

/************************************************************************/
/* PROGRAM DESCRIPTION
The program should add data to current packet (which is not transferring)
and try to send the packet if the PDC is available, if not available 
return(end).                                                            */
/************************************************************************/

struct debug_communication
{	
	Pdc *PdcInterface;
	
	volatile uint8_t CurrentTransferringPacket; // Should either be 0 or 1
	volatile uint8_t CurrentPacketFull;			// Flag

	volatile uint8_t Packet1[MAX_PACKET_LENGTH];
	volatile uint8_t Packet2[MAX_PACKET_LENGTH];

	volatile uint16_t Packet1Counter; // Number of bytes to send from packet 1
	volatile uint16_t Packet2Counter; // Number of bytes to send from packet 2
};

/************************************************************************/
/* The Message format should be "[TIME] STRING VALUE\n"
	TIME will be in most cases timer counter value of TC8
	STRING is a character array telling what the value is
	VALUE can be a 8 - 32 bit unsigned integer
IF and only IF the buffer (array) is full it should print 
	"[TIME] FULL !!!!!!\n" and raise a flag
When the buffer has been addressed to transport PDC, 
it should lower the flag. If the buffer is full and the PDC is 
transferring, the data will be discarded.                               */
/************************************************************************/

void debug_init_usart(struct debug_communication *Instance, Usart *UsartInstance, uint32_t BaudRate);
void debug_shitmix_init(struct debug_communication *Instance); // don't have time to make the general function 

void debug_new_instance(struct debug_communication *Instance, Pdc *PdcInterface);

void debug_transfer_new_message(struct debug_communication * Instance, uint32_t Time, char * String, uint32_t Value);

void debug_add_int_to_char_array(uint8_t *array, uint32_t data, uint16_t *counter);

uint8_t debug_write_usart_buffer(Usart *usart, uint32_t *buffer, uint32_t size);

void debug_add_to_packet(struct debug_communication * Instance, uint32_t Time, char * String, uint32_t Value, uint8_t * CurrentPacket);

uint8_t debug_peripheral_is_available(struct debug_communication * Instance);

void debug_send_message(struct debug_communication * Instance, uint8_t * CurrentPacket);

void debug_TEST_component();

#endif /* DEBUG_H_ */