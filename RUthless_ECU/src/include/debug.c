/*
 * debug.c
 *
 * Created: 17.6.2017 10:44:10
 *  Author: Jón Bjarni Bjarnason
 */ 

#include "debug.h"

void debug_init_usart(debug_communication * Instance, Usart * USART_Controller, uint32_t BaudRate)
{
	
}

void debug_transfer_new_message(debug_communication * Instance, uint32_t Time, char * String, uint32_t Value)
{
	
}

void debug_add_to_packet(debug_communication * Instance, uint32_t Time, char * String, uint32_t Value, uint8_t * CurrentPacket)
{
	
}

bool debug_peripheral_is_available(debug_communication * Instance)
{
	
}

void debug_send_message(debug_communication * Instance, uint8_t * CurrentPacket)
{
	
}

void debug_TEST_component()
{
	// Create instance
	debug_communication myInstance;
	Assert(myInstance);
	assert(myInstance);
}