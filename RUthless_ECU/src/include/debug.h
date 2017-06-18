/*
 * debug.h
 *
 * Created: 17.6.2017 10:44:31
 *  Author: Jón Bjarni Bjarnason
 */ 


#ifndef DEBUG_H_
#define DEBUG_H_

/************************************************************************/
/* The purpose of this file is to make an efficient debugging way to 
make debugging a part of the code without taking to much processor power.
This will be implemented by PDC (Peripheral DMA Controller), and should 
therefore be able to put the data to serial                             */
/************************************************************************/
#define MAX_PACKET_LENGTH 65536 // 16 bit uint

/************************************************************************/
/* PROGRAM DESCRIPTION
The program should add data to current packet (which is not transferring)
and try to send the packet if the PDC is available, if not available 
return(end).                                                            */
/************************************************************************/

typedef struct
{	
	Pdc *PdcInterface;
	
	uint8_t CurrentTransferringPacket; // Should either be 1 or 2

	uint8_t Packet1[MAX_PACKET_LENGTH];
	uint8_t Packet2[MAX_PACKET_LENGTH];

	uint16_t Packet1Counter; // Number of bytes to send from packet 1
	uint16_t Packet2Counter; // Number of bytes to send from packet 2
} debug_communication;


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

void debug_init_usart(debug_communication * Instance, Pdc *PdcInterface;, uint32_t BaudRate);

void debug_transfer_new_message(debug_communication * Instance, uint32_t Time, char * String, uint32_t Value);

void debug_add_to_packet(debug_communication * Instance, uint32_t Time, char * String, uint32_t Value, uint8_t * CurrentPacket);

bool debug_peripheral_is_available(debug_communication * Instance);

void debug_send_message(debug_communication * Instance, uint8_t * CurrentPacket);

void debug_TEST_component();

#endif /* DEBUG_H_ */