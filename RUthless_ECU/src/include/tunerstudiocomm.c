/*
 * tunerstudiocomm.c
 *
 * Created: 17.1.2017 15:17:39
 *  Author: jbbja
 */ 

#include "tunerstudiocomm.h"

// Initialization function
void tunerstudio_init(void)
{
	CurrPage = 255;
}

// Function to decide what to do when an character is received by UART
void tunerstudio_command(uint8_t character)
{
	switch (character)
	{
		case 'A':
			break;
		case 'B':
			break;
		case 'C':
			uart_interrupt_transfer(1);
			break;
		case 'F':
			uart_interrupt_transfer("001");
			break;
		case 'L':
			break;
		case 'N':
			break;
		case 'P':
			break;
		case 'R':
			break;
		case 'S':
			uart_interrupt_transfer("Speeduino 2016.12");
			break;
		case 'Q':
			uart_interrupt_transfer("speeduino 201612");
			break;
		case 'V':
			send_page();
			break;
		case 'W':
			break;
		case 'T':
			break;
		case 'r':
			break;
		case 't':
			break;
		case '?':
			break;
		default:
			break;
	}
	
}

// Function to send page according to the .ini file (CurrPage is used)
void send_page(void)
{
	switch(CurrPage)
	{
		case VE_PAGE:
			send_table()
		default:
			break;
	}
}

void send_3d_table(uint8_t table[THREE_D_TABLE_SIZE][THREE_D_TABLE_SIZE], uint8_t xbin[THREE_D_TABLE_SIZE], uint8_t xbin[THREE_D_TABLE_SIZE])
{
	
}