/*
 * global.c
 *
 * Created: 13.12.2016 15:02:04
 *  Author: jbbja
 */ 

#include "global.h"

void global_init(void)
{
	// DEBUG
	DebugCounter = 0;
	// TODO: Read from EEPROM

	// Frequency scalers for ADC, TODO: Finish telemetry
	GlobalTimerFreqADCScaler = 50;
	GlobalTimerFreqUARTScaler = 1;
	GlobalTimerFreqTelemetryScaler = 1;

	// Timer Variables
	CrankCurrCycleCounts = 0;
	CrankPrevCycleCounts = 0;
	CamCurrCycleCounts = 0;
	CrankTimerCounts = 0;
	CamTimerCounts = 0;
	CrankTooth = 1;
	CrankSignalFlag = FALSE;
	CamSignalFlag = FALSE;
	CrankTachCycleCounts = 0;
	TachPulse = 12; // Calculate new RPM every half of the trigger wheel 24 tooths

	storage_struct_read_eeprom_init(&engine_config2, sizeof(engine_config2), EEPROM_CONFIG2_INDEX);
	storage_struct_read_eeprom_init(&engine_config4, sizeof(engine_config4), EEPROM_CONFIG4_INDEX);
	storage_struct_read_eeprom_init(&engine_config6, sizeof(engine_config6), EEPROM_CONFIG6_INDEX);
	storage_struct_read_eeprom_init(&engine_config7, sizeof(engine_config7), EEPROM_CONFIG7_INDEX);
	storage_struct_read_eeprom_init(&engine_config8, sizeof(engine_config8), EEPROM_CONFIG8_INDEX);
	storage_struct_read_eeprom_init(&engine_config9, sizeof(engine_config9), EEPROM_CONFIG9_INDEX);

	storage_init_struct_to_zero(&engine_realtime, sizeof(engine_realtime));

	table_init();
	cylinder_init();
}

void cylinder_init(void)
{
	for (uint8_t i = 0; i < NR_OF_CYL; i++)
	{
		cylinder[i].IgnCntTimingOn = 0;
		cylinder[i].IgnCntTimingOff = 0;
		cylinder[i].InjCntTimingOn = 0;
		cylinder[i].InjCntTimingOff = 0;
	}
}