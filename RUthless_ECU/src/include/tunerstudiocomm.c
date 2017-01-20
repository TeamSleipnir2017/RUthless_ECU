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
	NewPageFlag = FALSE;
}

// Function to decide what to do when an character is received by UART
void tunerstudio_command(uint8_t character)
{
	// If 'P' came last time next character should contain the page number
	if (NewPageFlag)
	{
		CurrPage = character; //- '0';		// Withdraw the character value 0 (48 decimal in ASCII)
		NewPageFlag = FALSE;			// Reset the newpageflag
		return;							// Nothing else to do in this function
	}
	switch (character)
	{
		case 'A': // Real time data
			break;
		case 'B': // Burn command
			break;
		case 'C': // Not sure what this is
			uart_interrupt_transfer(1);
			break;
		case 'F': // Not sure what this is
			uart_interrupt_transfer("001");
			break;
		case 'L':
			break;
		case 'N':
			break;
		case 'P':
			NewPageFlag = TRUE;
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
			send_3d_table(VeTable, RpmBins, MapBins);
			break;
		default:
			break;
	}
	// Reset current page variable to prevent sending a wrong page, since CRC is not used
	CurrPage = 255;
}

void send_3d_table(uint8_t table[THREE_D_TABLE_SIZE][THREE_D_TABLE_SIZE], uint8_t xbin[THREE_D_TABLE_SIZE], uint8_t ybin[THREE_D_TABLE_SIZE])
{
	uint8_t transmit[THREE_D_TABLE_SIZE*THREE_D_TABLE_SIZE+THREE_D_TABLE_SIZE+THREE_D_TABLE_SIZE];
	uint16_t transmit_index = 0;
	// Load UART TX buffer with the Volumetric efficiency table 
	for (uint8_t i = 0; i < THREE_D_TABLE_SIZE; i++)
		for (uint8_t j = 0; j < THREE_D_TABLE_SIZE; j++)
			transmit[transmit_index++] = VeTable[i][j];
	// Load UART TX buffer with the RPM values for the Volumetric efficiency table 		
	for (uint8_t i = 0; i < THREE_D_TABLE_SIZE; i++)	
		transmit[transmit_index++] = RpmBins[i];
	// Load UART TX buffer with the MAP values for the Volumetric efficiency table
	for (uint8_t i = 0; i < THREE_D_TABLE_SIZE; i++)
		transmit[transmit_index++] = MapBins[i];
	// Let know when to stop
	transmit[transmit_index++] = NULL;
	// Let the UART interrupt handle sending the buffer
	uart_interrupt_transfer(transmit);		
}