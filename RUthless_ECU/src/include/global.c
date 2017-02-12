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
	CamCurrCycleCounts = 0;
	CrankTimerCounts = 0;
	CamTimerCounts = 0;
	CrankTooth = 1;
	CrankSignalFlag = FALSE;
	CamSignalFlag = FALSE;
	CrankTachCycleCounts = 0;
	TachPulse = 12; // Calculate new RPM every half of the trigger wheel 24 tooths

	engine_init();
	cylinder_init();
	engine_config_init();
}

void engine_init(void)
{
	engine.status = DEAD;				
	engine.Tps = 0;	
	engine.TpsAdc = 0;
	engine.LastTps = 0;	
	//engine.Tps2 = 0;				
	engine.Map = 0;				
	engine.Iat = 0;				
	engine.Clt = 0;				
	engine.Batt = 0;				
	engine.Afr = 0;			
	engine.LaunchControl = OFF;	
	engine.CurrVeTable = 0;
	engine.CurrAfrTable = 0;
	engine.CurrIgnTable = 0;
	engine.CurrRpm = 0;	
	engine.CurrSpeed = 0;			
	engine.CurrGear = 0;			
	engine.InjDuration = 0;		
	engine.IgnTiming = 0;			
	engine.IgnDwell = 0;			
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

void engine_config_init(void)
{
	engine_config.InjOpenTime		= eeprom_read_byte(EEPROM_INJ_OPEN_TIME_INDEX);
	
	engine_config.TpsLow			= eeprom_read_byte(EEPROM_TPS_LOW_INDEX);
	engine_config.TpsHigh			= eeprom_read_byte(EEPROM_TPS_HIGH_INDEX);		
	engine_config.TpsTimeDiff		= 0;		
	
	engine_config.Baro				= 0;	
	
	engine_config.MapLow			= eeprom_read_byte(EEPROM_MAP_LOW_INDEX);
	engine_config.MapHigh			= eeprom_read_int(EEPROM_MAP_HIGH_INDEX);		
	
	engine_config.RevLimit			= 0;			
	engine_config.LaunchControlRevLimit = 0;	
	
	engine_config.CltFanTemp		= 0;		
	engine_config.CltFanTempHyst	= 0;

	engine_config.TwiFault			= 0;
}