/*
 * global.c
 *
 * Created: 13.12.2016 15:02:04
 *  Author: jbbja
 */ 

#include "global.h"

void global_init(void)
{
	// TODO: Read from EEPROM
	GlobalTimerFreqADCScaler = 1;
	GlobalTimerFreqUARTScaler = 1;
	GlobalTimerFreqTelemetryScaler = 1;
}

void engine_init(void)
{
	engine.status = DEAD;				
	engine.Tps = 0;	
	engine.LastTps = 0;	
	//engine.Tps2 = 0;				
	engine.Map = 0;				
	engine.Iat = 0;				
	engine.Clt = 0;				
	engine.Batt = 0;				
	engine.O2 = 0;			
	engine.LaunchControl = OFF;	
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
	engine_config.TpsLow = 0;			
	engine_config.TpsHigh = 0;			
	engine_config.TpsTimeDiff = 0;		
	
	engine_config.Baro = 0;				
	
	engine_config.RevLimit = 0;			
	engine_config.LaunchControlRevLimit = 0;	
	
	engine_config.CltFanTemp = 0;		
	engine_config.CltFanTempHyst = 0;
}