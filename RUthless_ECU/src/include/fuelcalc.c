/*
 * fuelcalc.c
 *
 * Created: 1.2.2017 17:50:55
 *  Author: jbbja
 */ 

 #include "fuelcalc.h"

 //PV = nRT

uint32_t fuelcalc_pulsewidth(void)
{
	uint16_t Rpm = engine_realtime.Rpm, Map = engine_realtime.Map;
	int16_t Temperature = engine_realtime.Iat - TEMPERATURE_OFFSET;
	uint16_t Ve = math_interpolation_array(Rpm, Map, &VE);
	engine_realtime.VeTarget = Ve;
	//uart_print_string("Ve: "); uart_print_int(Ve); uart_new_line();
	uint16_t Afr = math_interpolation_array(Rpm, Map, &AFR);
	engine_realtime.AfrTarget = Afr;
	//uart_print_string("Afr: "); uart_print_int(Afr); uart_new_line();
	// Fuel equation:
	// Mair = (VE *  MAP * CYL_DISP) / (R (287 J/(Kg Kelvin)) * (IAT + 273))

	// MAP is in kPa, required to change it to Pa (1000) AND AFR is * 10, 13.0 = 130 AND VE is /100 since it is in % 
	// InjectorTime is in 100 nano seconds, InjectorTime = 10 = 1µs
	uint64_t Numerator = (uint64_t)Map * Ve * FUEL_CONST; 
	uint32_t Denominator = (uint32_t)Afr * (Temperature + 273);  
	uint64_t InjectorTime = Numerator / Denominator;
	return (uint32_t)InjectorTime;
}

/************************************************************************/
/* Gamma Enrichment is multiplied together, it consists of:
Warmup Enrichment
Afterstart Enrichment
                                                                     */
/************************************************************************/
uint16_t fuelcalc_GammaEnrich(void)
{
	uint16_t CoolantTemperature; 
	engine_config2.WarmUpEnrichPct;
}