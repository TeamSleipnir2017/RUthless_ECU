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
	WriteFlag = FALSE;
	Offset1 = 0;
	Offset2 = 0;
	OffsetFlag = FALSE;
}

// Function to decide what to do when an character is received by UART
void tunerstudio_command(uint8_t character)
{
	// If 'P' came last time next character should contain the page number
	if (NewPageFlag)
	{
		CurrPage = character; //- '0';	// Withdraw the character value 0 (48 decimal in ASCII)
		NewPageFlag = FALSE;			// Reset the newpageflag
		return;							// Nothing else to do in this function
	}
	// If 'W' came last time next character should contain the offset
	/************************************************************************/
	/* Write command
	P (page number) W (line, column (per nibble)) 0 (Data) */
	/************************************************************************/
	if (WriteFlag)
	{
		if (OffsetFlag == 0){		// Receive first offset value
			Offset1 = character;
			OffsetFlag = 1;
		}
		else if (OffsetFlag == 1){	// Receive second offset value
			Offset2 = character;
			OffsetFlag = 2;
		}
		else if (OffsetFlag == 2){	// Receive data to write
			tunerstudio_write_data(character);
			Offset1 = Offset2 = OffsetFlag = WriteFlag = 0;
		}
	}
	
	switch (character)
	{
		case 'A': // Real time data
			tunerstudio_send_dummy_data(37, 100);
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
			tunerstudio_send_page();
			break;
		case 'W':
			WriteFlag = TRUE;
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
void tunerstudio_send_page(void)
{
	switch(CurrPage)
	{
		case VE_PAGE:
			tunerstudio_send_3d_table(VeTable, VeRpmBins, VeMapBins);
			break;
		case 2:
			tunerstudio_send_dummy_data(64, 255);
			break;
		case IGN_PAGE:
			tunerstudio_send_3d_table(IgnTable, IgnRpmBins, IgnMapBins);
			break;
		case 4:
			tunerstudio_send_dummy_data(64, 255);
			break;
		case AFR_PAGE:
			tunerstudio_send_3d_table(AfrTable, AfrRpmBins, AfrMapBins);
			break;
		case 6:
			tunerstudio_send_dummy_data(64, 255);
			break;
		case 7:
			tunerstudio_send_dummy_data(64, 255);
			break;
		case 8:
			tunerstudio_send_dummy_data(160, 255);
			break;
		case 9:
			tunerstudio_send_dummy_data(192, 255);
			break;
		default:
			break;
	}
	// Reset current page variable to prevent sending a wrong page, since CRC is not used
	// When writing, tunerstudio only sends first time page number
	//CurrPage = 255;
}

void tunerstudio_send_3d_table(uint8_t table[THREE_D_TABLE_SIZE][THREE_D_TABLE_SIZE], uint8_t xbin[THREE_D_TABLE_SIZE], uint8_t ybin[THREE_D_TABLE_SIZE])
{
	uint8_t transmit[THREE_D_TABLE_SIZE*THREE_D_TABLE_SIZE+THREE_D_TABLE_SIZE+THREE_D_TABLE_SIZE];
	uint16_t transmit_index = 0;
	// Load UART TX buffer with the Volumetric efficiency table 
	for (uint8_t i = 0; i < THREE_D_TABLE_SIZE; i++)
		for (uint8_t j = 0; j < THREE_D_TABLE_SIZE; j++)
			transmit[transmit_index++] = table[i][j];
	// Load UART TX buffer with the RPM values for the Volumetric efficiency table 		
	for (uint8_t i = 0; i < THREE_D_TABLE_SIZE; i++)	
		transmit[transmit_index++] = xbin[i];

	// Load UART TX buffer with the MAP values for the Volumetric efficiency table
	for (uint8_t i = 0; i < THREE_D_TABLE_SIZE; i++)
		transmit[transmit_index++] = ybin[i];

	// Let know when to stop
	transmit[transmit_index++] = NULL;
	// Let the UART interrupt handle sending the buffer
	uart_interrupt_transfer(transmit);		
}

void tunerstudio_send_dummy_data(uint16_t NumberOfBytes, uint8_t dummy)
{
	uint8_t transmit[NumberOfBytes];
	uint16_t i = 0;
	for (; i < NumberOfBytes; i++)
		transmit[i] = dummy;
	transmit[i++] = NULL;
	uart_interrupt_transfer(transmit);
}

void tunerstudio_write_data(uint8_t data)
{
	switch (CurrPage)
	{
	case VE_PAGE:
		tunerstudio_write_to_table(data, VeTable, VeRpmBins, VeMapBins);
		break;
	case IGN_PAGE:
		tunerstudio_write_to_table(data, IgnTable, IgnRpmBins, IgnMapBins);
		break;
	case AFR_PAGE:
		tunerstudio_write_to_table(data, AfrTable, AfrRpmBins, AfrMapBins);
		break;
	}
}

void tunerstudio_write_to_table(uint8_t table[THREE_D_TABLE_SIZE][THREE_D_TABLE_SIZE], uint8_t xbin[THREE_D_TABLE_SIZE], uint8_t ybin[THREE_D_TABLE_SIZE], uint8_t data)
{
	if (!Offset2){ // Write data to table
		table[Offset1 & 0x0f][Offset1 >> 4] = data;
	}
	else {
		if (Offset1 < THREE_D_TABLE_SIZE){
			xbin[Offset1] = data;
		}
		else{
			ybin[Offset1 - THREE_D_TABLE_SIZE] = data;
		}
	}
	// DEBUGG
	at24cxx_write_byte(Offset1, data);
}