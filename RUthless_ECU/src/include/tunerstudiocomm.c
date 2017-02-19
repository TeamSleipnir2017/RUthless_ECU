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
	TestFlag = FALSE;
	for (uint16_t i = 0; i < DATA_LENGTH; i++)
	{
		TestBuffer[i] = 255;
	}
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
		else if ((CurrPage == VE_PAGE || CurrPage == AFR_PAGE || CurrPage == IGN_PAGE) && OffsetFlag != 2){	// Receive second offset value IF the current page is a table (VE, AFR, IGN)
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
		case 'A': // Real time data
			tunerstudio_send_real_time_data();
			//tunerstudio_send_dummy_data(37, 100);
			break;
		case 'B': // Burn command
			tunerstudio_burn_page_eeprom();
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
			uart_interrupt_transfer("RUthless V1.0 Formula Student 2017");
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
			uart_disable_rx_interrupt();			// Read manually from buffer simplifies this since it is not necessary to do this in realtime
			tunerstudio_update_calib_vect();
			uart_enable_rx_interrupt();
			break;
		case '?':
			break;
		case 'D':
			tunerstudio_debug_global_function();
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
		case CONFIG_PAGE:
			tunerstudio_send_config(&engine_config2, ENGINE_CONFIG2_LEN);
			//tunerstudio_send_config1();
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

	// Let the UART interrupt handler send the buffer
	uart_interrupt_transfer_specific(transmit, transmit_index);		
}
 
// ATTENTION TODO: optimize speed by loading buffer straight instead of creating array and use for loop twice
void tunerstudio_send_config1(void)
{
	#define LEN 72
	uint8_t transmit[LEN];
	// Initialize array with constant, since all of the variables are not used
	uint16_t i = 0;
	for (; i < LEN; i++)
		transmit[i] = 255;

	transmit[i++] = NULL;
	uart_interrupt_transfer(transmit);
}

void tunerstudio_send_config(uint8_t *ConfigStructPointer, uint16_t ConfigLen)
{
	uint8_t transmit[ConfigLen];
	for (uint16_t i = 0; i < ConfigLen; i++)
		transmit[i] = *((uint8_t *)ConfigStructPointer + i);
	uart_interrupt_transfer_specific(transmit, ConfigLen);
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

void tunerstudio_write_data(uint16_t data)
{
	switch (CurrPage)
	{
	case VE_PAGE:
		tunerstudio_write_to_table(data, VeTable, VeRpmBins, VeMapBins);
		break;
	case CONFIG_PAGE:
		tunerstudio_write_config(&engine_config2, data);
// 		switch (Offset1)
// 		{
// 		case CONFIG_TPS_LOW_OFFSET:
// 			engine_config.TpsLow = data; break;
// 		case CONFIG_TPS_HIGH_OFFSET:
// 			engine_config.TpsHigh = data; break;
// 		case CONFIG_MAP_LOW_OFFSET:
// 			engine_config.MapLow = data; break;
// 		case CONFIG_MAP_HIGH_OFFSET_FIRST_BYTE:
// 			engine_config.MapHigh = data; break;
// 		case CONFIG_MAP_HIGH_OFFSET_SECOND_BYTE:
// 			engine_config.MapHigh |= data << 8; 
// 			break;
// 		default:
// 			break;
// 		}
		break;
	case IGN_PAGE:
		tunerstudio_write_to_table(data, IgnTable, IgnRpmBins, IgnMapBins);
		break;
	case AFR_PAGE:
		tunerstudio_write_to_table(data, AfrTable, AfrRpmBins, AfrMapBins);
		break;
	default:
		break;
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

void tunerstudio_write_config(uint8_t *ConfigStructPointer, uint8_t data)
{
	*(ConfigStructPointer + Offset1) = data;
}

void tunerstudio_burn_page_eeprom(void)
{
	switch (CurrPage)
	{
	case VE_PAGE:
		tunerstudio_burn_table_eeporm(EEPROM_VE_INDEX ,VeTable, VeRpmBins, VeMapBins);
		break;
	case IGN_PAGE:
		tunerstudio_burn_table_eeporm(EEPROM_IGN_INDEX, IgnTable, IgnRpmBins, IgnMapBins);
		break;
	case AFR_PAGE:
		tunerstudio_burn_table_eeporm(EEPROM_AFR_INDEX, AfrTable, AfrRpmBins, AfrMapBins);
		break;
	case CONFIG_PAGE:
		tunerstudio_burn_config(&engine_config2, ENGINE_CONFIG2_LEN, EEPROM_CONFIG2_INDEX);
		//tunerstudio_burn_config1();
		break;
	default:
		break;
	}
}

void tunerstudio_burn_table_eeporm(uint16_t EepromIndex, uint8_t table[THREE_D_TABLE_SIZE][THREE_D_TABLE_SIZE], uint8_t xbin[THREE_D_TABLE_SIZE], uint8_t ybin[THREE_D_TABLE_SIZE])
{
	if (engine_config.TwiFault == TRUE) // check if communication with EEPROM is okay
		return;
	
	// Compare current table with the data in EEPROM
	for (uint8_t i = 0; i < THREE_D_TABLE_SIZE; i++) 
		for (uint8_t j = 0; j < THREE_D_TABLE_SIZE; j++)
			tunerstudio_burn_value_if_changed(table[i][j], EepromIndex++);

	// Load UART TX buffer with the RPM values for the Volumetric efficiency table
	for (uint8_t i = 0; i < THREE_D_TABLE_SIZE; i++) 
		tunerstudio_burn_value_if_changed(xbin[i], EepromIndex++);

	// Load UART TX buffer with the MAP values for the Volumetric efficiency table
	for (uint8_t i = 0; i < THREE_D_TABLE_SIZE; i++) 
		tunerstudio_burn_value_if_changed(ybin[i], EepromIndex++);
}

void tunerstudio_burn_value_if_changed(uint32_t TempValue, uint32_t EepromIndex)
{
	if (eeprom_read_byte(EepromIndex) != TempValue){ // Compare current value with EEPROM value, update EEPROM if it is not the same
		at24cxx_write_byte(EepromIndex, TempValue);
	}
}

void tunerstudio_burn_config1(void)
{
	if (engine_config.TwiFault == TRUE) // check if communication with EEPROM is okay
		return;
	tunerstudio_burn_value_if_changed(engine_config.InjOpenTime		, EEPROM_INJ_OPEN_TIME_INDEX);
	tunerstudio_burn_value_if_changed(engine_config.TpsLow			, EEPROM_TPS_LOW_INDEX);	
	tunerstudio_burn_value_if_changed(engine_config.TpsHigh			, EEPROM_TPS_HIGH_INDEX);		
	tunerstudio_burn_value_if_changed(engine_config.MapLow			, EEPROM_MAP_LOW_INDEX);	
	tunerstudio_burn_value_if_changed(engine_config.MapHigh & 0xFF	, EEPROM_MAP_HIGH_INDEX);	// Write first byte
	tunerstudio_burn_value_if_changed(engine_config.MapHigh >> 8	, EEPROM_MAP_HIGH_INDEX + 1);// Write second byte
}
void tunerstudio_burn_config(uint8_t *ConfigStructPointer, uint16_t ConfigLen, uint16_t EepromIndex)
{
	if (engine_config.TwiFault == TRUE) // check if communication with EEPROM is okay
	{	
		uart_print_string("EEPROM Fault");
		return;
	}
	for (uint16_t i = 0; i < ConfigLen; i++)
	{
// 		uint8_t var = *((uint8_t *)ConfigStructPointer + i);
// 		uint8_t EEvar = eeprom_read_byte(EEPROM_CONFIG2_INDEX + i);
// 		if (EEvar != var){ // Compare current value with EEPROM value, update EEPROM if it is not the same
// 			uart_print_string("var: "); uart_print_int(var);
// 			uart_print_string("EE: "); uart_print_int(EEvar);
// 			at24cxx_write_byte(EEPROM_CONFIG2_INDEX + i, var);
// 		}
		
		
		tunerstudio_burn_value_if_changed(*((uint8_t *)ConfigStructPointer + i), EEPROM_CONFIG2_INDEX + i);
	}
}

void tunerstudio_send_real_time_data(void)
{
	uint8_t transmit[NUMBER_OF_REAL_TIME_BYTES];
	// Initialize array with constant, since all of the variables are not used
	for (uint16_t i = 0; i < NUMBER_OF_REAL_TIME_BYTES; i++)
		transmit[i] = 100;
	// Load IAT value to transmission buffer and so on
	transmit[REALTIME_MAP_INDEX] = engine.Map >> 1; // divide by 2
	transmit[REALTIME_IAT_INDEX] = engine.Iat;
	transmit[REALTIME_CLT_INDEX] = engine.Clt;
	transmit[REALTIME_TPS_ADC_INDEX] = engine.TpsAdc;
	transmit[REALTIME_BAT_INDEX] = engine.Batt;
	transmit[REALTIME_AFR_INDEX] = engine.Afr;
	transmit[REALTIME_RPM_INDEX] = engine.CurrRpm;
	transmit[REALTIME_RPM_INDEX + 1] = (engine.CurrRpm >> 8);
	transmit[REALTIME_CURRVE_INDEX] = engine.CurrVeTable;
	transmit[REALTIME_CURRAFR_INDEX] = engine.CurrAfrTable;
	transmit[REALTIME_PW_INDEX] = engine.InjDuration / 100000; // convert to ms * 10
	transmit[REALTIME_IGN_INDEX] = engine.IgnTiming; 
	transmit[REALTIME_TPS_INDEX] = engine.Tps;

	// Transfer buffer
	uart_interrupt_transfer_specific(transmit, NUMBER_OF_REAL_TIME_BYTES);
}

void tunerstudio_update_calib_vect(void)
{
	// Check if the interrupt buffer consists of data
	if (RxStringTail != RxStringHead)
	{
		// TODO: Read the buffer and then continue, possibly not necessary
		uart_print_string("ERROR");
		RxStringTail = RxStringHead = 0;
	}
	else
	{
		uint8_t receive = uart_receive();
		switch (receive)
		{
			case CONFIG_CLT:
				tunerstudio_update_calib_vect_helper(2, EEPROM_CLT_ADC_INDEX);
				break;
			case CONFIG_IAT:
				tunerstudio_update_calib_vect_helper(2, EEPROM_IAT_ADC_INDEX);
				break;
			case CONFIG_AFR:
				tunerstudio_update_calib_vect_helper(1, EEPROM_AFR_ADC_INDEX);
				break;
			default:
				break;
		}
	}
}
void tunerstudio_update_calib_vect_helper(uint8_t NrOfBytes, uint32_t EepromIndex)
{
	uint8_t receive[2];			// Receiving buffer
	receive[0] = receive[1] = 0;
	int16_t TempValue = 0;		// Integer, the value can be negative
	for (uint16_t i = 0; i < DATA_LENGTH; i++)
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
	uart_print_string("MAP High: "); uart_print_int(engine_config.MapHigh); uart_new_line();
	uart_print_string("MAP Low: "); uart_print_int(engine_config.MapLow); uart_new_line();
	uart_print_string("TPS High: "); uart_print_int(engine_config.TpsHigh); uart_new_line();
	uart_print_string("TPS Low: "); uart_print_int(engine_config.TpsLow); uart_new_line();
	uart_print_string("CLT: "); uart_print_int(engine.Clt); uart_new_line();
	uart_print_string("IAT: "); uart_print_int(engine.Iat); uart_new_line();
	uart_print_string("AFR: "); uart_print_int(engine.Afr); uart_new_line();
	uart_print_string("MAP: "); uart_print_int(engine.Map); uart_new_line();
	uart_print_string("TPS: "); uart_print_int(engine.Tps); uart_new_line();
	uart_print_string("RPM: "); uart_print_int(engine.CurrRpm); uart_new_line();
	uart_print_string("TWIFault: "); uart_print_int(engine_config.TwiFault); uart_new_line();
	uart_print_string("Fuel Const: "); uart_print_int(FUEL_CONST); uart_new_line();
}