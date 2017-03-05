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
			return;
		}
		else if ((CurrPage == PAGE_VE || CurrPage == PAGE_AFR || CurrPage == PAGE_IGN) && OffsetFlag != 2)
		{	// Receive second offset value IF the current page is a table (VE, AFR, IGN) (Since the tables have more than 256 values see .ini file)
			Offset2 = character;
			OffsetFlag = 2;
			return;
		}
		else if (OffsetFlag){	// Receive data to write
			tunerstudio_write_data(character);
			Offset1 = Offset2 = OffsetFlag = WriteFlag = 0;
			return;
		}
	}
	
	switch (character)
	{
		case 'A': 
		{
			engine_realtime.Seconds = millis / MILLI_SEC;
			tunerstudio_send_struct_pdc(&engine_realtime, sizeof(engine_realtime));	
			//tunerstudio_send_struct(&engine_realtime, sizeof(engine_realtime));	
			break;
		}
		case 'B': tunerstudio_burn_page_eeprom();										break;
		case 'C': uart_interrupt_transfer(1);											break;
		case 'F': uart_interrupt_transfer("001");										break;
		case 'L':																		break;
		case 'N':																		break;
		case 'P': NewPageFlag = TRUE;													break;
		case 'R':																		break;
		case 'S': uart_interrupt_transfer("RUthless V1.0 Formula Student 2017");		break;
		case 'Q': uart_interrupt_transfer("speeduino");									break;
		case 'V': tunerstudio_send_page(); 												break;
		case 'W': WriteFlag = TRUE;														break;
		case 'T':																		break;
		case 'r':																		break;
		case 't':
			uart_disable_rx_interrupt();			// Read manually from buffer simplifies this since it is not necessary to do this in realtime
			tunerstudio_update_calib_vect();
			uart_enable_rx_interrupt();
			break;
		case '?': break;
		case 'D': tunerstudio_debug_global_function(); break;
		default: break;
	}
	
}

// Function to send page according to the .ini file (CurrPage is used)
void tunerstudio_send_page(void)
{
	switch(CurrPage)
	{
		case PAGE_VE:		tunerstudio_send_Table3D(&VE);										break;
		case PAGE_CONFIG2:	tunerstudio_send_struct(&engine_config2, sizeof(engine_config2));	break;
		case PAGE_IGN:		tunerstudio_send_Table3D(&IGN);										break;
		case PAGE_CONFIG4:	tunerstudio_send_struct(&engine_config4, sizeof(engine_config4));	break;
		case PAGE_AFR:		tunerstudio_send_Table3D(&AFR);										break;
		case PAGE_CONFIG6:	tunerstudio_send_struct(&engine_config6, sizeof(engine_config6));	break;
		case PAGE_CONFIG7:	tunerstudio_send_struct(&engine_config7, sizeof(engine_config7));	break;
		case PAGE_CONFIG8:	tunerstudio_send_struct(&engine_config8, sizeof(engine_config8));	break;
		case PAGE_CONFIG9:	tunerstudio_send_struct(&engine_config9, sizeof(engine_config9));	break;
		default: break;
	}
}

void tunerstudio_send_Table3D(struct Table3D *Current)
{
	for (uint8_t i = 0; i < Current->Ysize; i++)
		tunerstudio_send_struct(Current->Table[i], Current->Xsize);
	tunerstudio_send_struct(Current->Xbin, Current->Xsize);
	tunerstudio_send_struct(Current->Ybin, Current->Ysize);
}
 

void tunerstudio_send_struct(uint8_t *ConfigStructPointer, uint16_t ConfigLen)
{
	for (uint16_t i = 0; i < ConfigLen; i++)
		uart_load_tx_buffer(*((uint8_t *)ConfigStructPointer + i));
	uart_enable_tx_interrupt();
}

void tunerstudio_send_struct_pdc(uint8_t *ConfigStructPointer, uint16_t ConfigLen)
{
	uart_load_pdc_tx_buffer(ConfigStructPointer, ConfigLen);
}

void tunerstudio_write_data(uint16_t data)
{
	switch(CurrPage)
	{
		case PAGE_VE:		tunerstudio_write_Table3D(&VE, data);				break;
		case PAGE_CONFIG2:	tunerstudio_write_struct(&engine_config2, data);	break;
		case PAGE_IGN:		tunerstudio_write_Table3D(&IGN, data);				break;
		case PAGE_CONFIG4:	tunerstudio_write_struct(&engine_config4, data);	break;
		case PAGE_AFR:		tunerstudio_write_Table3D(&AFR, data);				break;
		case PAGE_CONFIG6:	tunerstudio_write_struct(&engine_config6, data);	break;
		case PAGE_CONFIG7:	tunerstudio_write_struct(&engine_config7, data);	break;
		case PAGE_CONFIG8:	tunerstudio_write_struct(&engine_config8, data);	break;
		case PAGE_CONFIG9:	tunerstudio_write_struct(&engine_config9, data);	break;
		default: break;
	}
}

void tunerstudio_write_to_table(uint8_t data, uint8_t table[THREE_D_TABLE_SIZE][THREE_D_TABLE_SIZE], uint8_t xbin[THREE_D_TABLE_SIZE], uint8_t ybin[THREE_D_TABLE_SIZE])
{
	// table[line][column]
	if (!Offset2){ // Write data to table
		table[Offset1 >> 4][Offset1 & 0x0f] = data;
	}
	else {
		if (Offset1 < THREE_D_TABLE_SIZE){
			xbin[Offset1] = data;
		}
		else{
			ybin[Offset1 - THREE_D_TABLE_SIZE] = data;
		}
	}
}

void tunerstudio_write_Table3D(struct Table3D *Current, uint8_t data)
{
	// table[line][column]
	if (!Offset2){ // Write data to table
		Current->Table[Offset1 >> 4][Offset1 & 0x0f] = data;
	}
	else 
	{
		if (Offset1 < THREE_D_TABLE_SIZE)
		{
			Current->Xbin[Offset1] = data;
		}
		else
		{
			Current->Ybin[Offset1 - THREE_D_TABLE_SIZE] = data;
		}
	}
}

void tunerstudio_write_struct(uint8_t *ConfigStructPointer, uint8_t data)
{
	*(ConfigStructPointer + Offset1) = data;
}

void tunerstudio_burn_page_eeprom(void)
{
	switch(CurrPage)
	{
		case 1: tunerstudio_burn_Table3D(&VE, EEPROM_VE_INDEX);											break;
		case 2: tunerstudio_burn_struct(&engine_config2, sizeof(engine_config2), EEPROM_CONFIG2_INDEX);	break;
		case 3: tunerstudio_burn_Table3D(&IGN, EEPROM_IGN_INDEX);										break;
		case 4: tunerstudio_burn_struct(&engine_config4, sizeof(engine_config4), EEPROM_CONFIG4_INDEX);	break;
		case 5: tunerstudio_burn_Table3D(&AFR, EEPROM_AFR_INDEX);										break;
		case 6: tunerstudio_burn_struct(&engine_config6, sizeof(engine_config6), EEPROM_CONFIG6_INDEX);	break;
		case 7: tunerstudio_burn_struct(&engine_config7, sizeof(engine_config7), EEPROM_CONFIG7_INDEX);	break;
		case 8: tunerstudio_burn_struct(&engine_config8, sizeof(engine_config8), EEPROM_CONFIG8_INDEX);	break;
		case 9: tunerstudio_burn_struct(&engine_config9, sizeof(engine_config9), EEPROM_CONFIG9_INDEX);	break;
		default: break;
	}
}

void tunerstudio_burn_Table3D(struct Table3D *Current, uint16_t EepromIndex)
{
	if (engine_realtime.TwiFault == TRUE) // check if communication with EEPROM is okay
	{
		uart_print_string("EEPROM Fault");
		return;
	}
	for (uint8_t i = 0; i < Current->Ysize; i++)
	{
		tunerstudio_burn_struct(Current->Table[i], Current->Xsize, EepromIndex);
		EepromIndex += Current->Xsize;
	}
	tunerstudio_burn_struct(Current->Xbin, Current->Xsize, EepromIndex);
	EepromIndex += Current->Xsize;
	tunerstudio_burn_struct(Current->Ybin, Current->Ysize, EepromIndex);
}

void tunerstudio_burn_value_if_changed(uint32_t TempValue, uint16_t EepromIndex)
{
	if (eeprom_read_byte(EepromIndex) != TempValue){ // Compare current value with EEPROM value, update EEPROM if it is not the same
		at24cxx_write_byte(EepromIndex, TempValue);
	}
}

void tunerstudio_burn_struct(uint8_t *ConfigStructPointer, uint16_t ConfigLen, uint16_t EepromIndex)
{
	if (engine_realtime.TwiFault == TRUE) // check if communication with EEPROM is okay
	{	
		uart_print_string("EEPROM Fault");
		return;
	}
	for (uint16_t i = 0; i < ConfigLen; i++)
		tunerstudio_burn_value_if_changed(*((uint8_t *)ConfigStructPointer + i), EepromIndex + i);
}

void tunerstudio_update_calib_vect(void)
{
	// Check if the interrupt buffer consists of data
	if (RxStringTail != RxStringHead)
	{
		// TODO: Read the buffer and then continue, possibly not necessary
		uart_print_string("ERROR calibration vector");
		RxStringTail = RxStringHead = 0;
	}
	else
	{
		uint8_t receive = uart_receive();
		switch (receive)
		{
			case CONFIG_CLT: tunerstudio_update_calib_vect_helper(2, EEPROM_CLT_ADC_INDEX); break;
			case CONFIG_IAT: tunerstudio_update_calib_vect_helper(2, EEPROM_IAT_ADC_INDEX); break;
			case CONFIG_AFR: tunerstudio_update_calib_vect_helper(1, EEPROM_AFR_ADC_INDEX); break;
			default: break;
		}
	}
}
void tunerstudio_update_calib_vect_helper(uint8_t NrOfBytes, uint16_t EepromIndex)
{
	uint8_t receive[2];			// Receiving buffer
	receive[0] = receive[1] = 0;
	int16_t TempValue = 0;		// Integer, the value can be negative
	for (uint16_t i = 0; i < LUT_SIZE; i++)
	{
		receive[0] = uart_receive();
		if (NrOfBytes == 1) {		
			TempValue = receive[0];		// AFR
		}
		else {
			receive[1] = uart_receive();
			TempValue = (int)((receive[1] << 8) | (receive[0]));// Put together the temperature value
			TempValue = ((TempValue - 320)  * 5) / 9;			// Change to celsius from fahrenheit
			TempValue /= 10;									// Tunerstudio sends the value scaled up by 10
			TempValue += TEMPERATURE_OFFSET;					// Offset to discard the need of negative integer
		}
		if (TempValue > 255){ // Store only bytes in EEPROM
			TempValue = 255;
		} 
		else if (TempValue < 0){ // For safety
			TempValue = 0;
		}
		at24cxx_write_byte(EepromIndex++, TempValue);
	}
}

void tunerstudio_debug_global_function(void)
{
	uart_print_string("MAP High: "); uart_print_int(engine_config2.MapMax); uart_new_line();
	uart_print_string("MAP Low: "); uart_print_int(engine_config2.MapMin); uart_new_line();
	uart_print_string("TPS High: "); uart_print_int(engine_config2.TpsMax); uart_new_line();
	uart_print_string("TPS Low: "); uart_print_int(engine_config2.TpsMin); uart_new_line();
	uart_print_string("CLT: "); uart_print_int(engine_realtime.Clt); uart_new_line();
	uart_print_string("IAT: "); uart_print_int(engine_realtime.Iat); uart_new_line();
	uart_print_string("AFR: "); uart_print_int(engine_realtime.Afr); uart_new_line();
	uart_print_string("MAP: "); uart_print_int(engine_realtime.Map); uart_new_line();
	uart_print_string("TPS: "); uart_print_int(engine_realtime.Tps); uart_new_line();
	uart_print_string("RPM: "); uart_print_int(engine_realtime.Rpm); uart_new_line();
	uart_print_string("TWIFault: "); uart_print_int(engine_realtime.TwiFault); uart_new_line();
	uart_print_string("Fuel Const: "); uart_print_int(FUEL_CONST); uart_new_line();
	uart_print_string("AfterStartEnrichPct: "); uart_print_int(engine_config2.AfterStartEnrichPct); uart_new_line();
	uart_print_string("AfterStartEnrichCycles: "); uart_print_int(engine_config2.AfterStartEnrichSec); uart_new_line();
	uart_print_string("millis: "); uart_print_int(millis); uart_new_line();

}