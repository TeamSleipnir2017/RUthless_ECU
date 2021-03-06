/*
 * igncalc.c
 *
 * Created: 22.3.2017 15:24:07
 *  Author: rikardur
 */ 

#include "igncalc.h"


// Calculate the amount of teeth to skip for the next ignition event
// Returns an integer to skip
// Example: Skip 7,4 teeth ---> the function returns the integer 7
uint16_t igncalc_ign_time_teeth(uint16_t ign_degree)
{
	uint32_t temp = ((CRANK_DEGREE_INTERVAL * 10 - ign_degree) * 10) / CRANK_DEGREE_INTERVAL;
	uint32_t temp1 = (temp * (CRANK_TEETH/TACH_EVENTS))/100;
	
// 	if (isDebug)
// 	{
// 		uart_transfer('A'); uart_print_int(CRANK_DEGREE_INTERVAL); uart_new_line();
// 		uart_transfer('B'); uart_print_int(ign_degree); uart_new_line();
// 		uart_transfer('C'); uart_print_int(temp); uart_new_line();
// 		uart_transfer('R'); uart_print_int(temp1); uart_new_line();
// 	}
	
	return	temp1;
	
	
// 	if (IgnATDC)
// 	{
// 		IgnATDC = FALSE;
// 		return	((((CRANK_DEGREE_INTERVAL * 10 + ign_degree) * 10) / CRANK_DEGREE_INTERVAL) * CRANK_TEETH)/TACH_EVENTS/100;
// 	}
// 	else
// 	{
// 		uint32_t temp = ((CRANK_DEGREE_INTERVAL - ign_degree/10) * 10) / CRANK_DEGREE_INTERVAL;
// 		uint32_t temp1 = (temp * (CRANK_TEETH/TACH_EVENTS))/100;
// 	
// 		if (isDebug)
// 		{
// 			uart_transfer('o'); uart_print_int(temp); uart_new_line();
// 			uart_transfer('R'); uart_print_int(temp1); uart_new_line();
// 		}
// 		
// 		return	temp1;
// 	}
}

// Calculate the time between 2 teeth
// Returns the counts to interval from the current tooth
// Example: Skip 7,4 teeth ---> the function uses the decimal number 0,4 to calculate the spark event
uint32_t igncalc_ign_time_interval(uint16_t ign_degree)
{
	uint32_t temp1;
	if (IgnATDC)
	{
		temp1 = (((CRANK_DEGREE_INTERVAL * 10 + ign_degree) * 10) / CRANK_DEGREE_INTERVAL) * (CRANK_TEETH/TACH_EVENTS) - (igncalc_ign_time_teeth(ign_degree) * 100);
	}
	else
	{
		temp1 = (((CRANK_DEGREE_INTERVAL * 10 - ign_degree) * 10) / CRANK_DEGREE_INTERVAL) * (CRANK_TEETH/TACH_EVENTS) - (igncalc_ign_time_teeth(ign_degree) * 100);
	}
	
	return (temp1 * CrankPrevCycleCounts)/100;
}


// Calculate how much dwell is needed
// Returns dwell in counts
uint32_t igncalc_dwell_degree(void)
{
	// TODO: if cranking then use engine_config4.DwellTimeCranking
	uint8_t correction = igncalc_dwell_correction();
	uint16_t totalDwellms = (engine_config4.DwellTimeRunning * correction)/100; // tenths of milliseconds 

	if ((totalDwellms > (engine_config4.DwellLimit * 10)) && engine_config4.DwellLimitEnable)
	{
		totalDwellms = engine_config4.DwellLimit * 10;
	}
	engine_realtime.Dwell = totalDwellms;
	uint32_t temp2 = ((GLOBAL_TIMER_FREQ/1000)*totalDwellms)/10;
	
	//return (3600*temp2)/CrankRevCounts; changed to below 5.4.17 JBB
	return (3600*temp2)/LastCrankRevCounts;
}

// Calculate how much dwell is needed
// Returns dwell in hundreds of nanosecond, 1 = 100ns
uint32_t igncalc_dwell_pulsewidth(void)
{
	uint16_t correction = igncalc_dwell_correction(); // 0 - 1000 represents 0 - 100.0%
	uint32_t TotalDwellus;
	if (engine_realtime.Rpm < engine_config2.CrankingRpm)
	{
		TotalDwellus = (engine_config4.DwellTimeCranking * correction); // return hundred of nanoseconds 
	}
	else
	{
		TotalDwellus = (engine_config4.DwellTimeRunning * correction); // return hundred of nanoseconds
	}
	 
	if (engine_config4.DwellLimitEnable && (TotalDwellus > IgnitionDwellLimit))
	{
		TotalDwellus = IgnitionDwellLimit;
	}
	engine_realtime.Dwell = TotalDwellus/1000;

	return TotalDwellus;
}


// Correction for dwell in regards to battery voltage
// Returns the dwell correction in percentage (%) 0.0 - 255.0% depending on the parameters set in TunerStudio (Scaled up by 0.1), 100.0% = 1000 
uint16_t igncalc_dwell_correction(void)
{
	uint8_t voltage = engine_realtime.BattVolt;
	// Use scaler 23.4.17 JBB
	uint32_t totalDwell = math_interpolation_vector(&engine_config6.BattRefVoltBins, &engine_config4.DwellCorrectionValues, voltage, 10, sizeof(engine_config4.DwellCorrectionValues));
	//uint32_t totalDwell = math_interpolation_vector(&engine_config6.BattRefVoltBins, &engine_config4.DwellCorrectionValues, voltage, 1, sizeof(engine_config4.DwellCorrectionValues));
	return totalDwell;
}




