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
	uint16_t Rpm = engine.CurrRpm, Map = engine.Map;
	int16_t Temperature = engine.Iat - TEMPERATURE_OFFSET;
	uint16_t Ve = math_interpolation_array(Rpm, Map, &VeTable, &VeRpmBins, &VeMapBins);
	engine.CurrVeTable = Ve;
	//uart_print_string("Ve: "); uart_print_int(Ve); uart_new_line();
	uint16_t Afr = math_interpolation_array(Rpm, Map, &AfrTable, &AfrRpmBins, &AfrMapBins);
	engine.CurrAfrTable = Afr;
	//uart_print_string("Afr: "); uart_print_int(Afr); uart_new_line();
	// Fuel equation:
	// Mair = (VE *  MAP * CYL_DISP) / (R (287 J/(Kg Kelvin)) * (IAT + 273))

	// MAP is in kPa, required to change it to Pa (1000) AND AFR is * 10, 13.0 = 130 AND VE is /100 since it is in % 
	// InjectorTime is in nano seconds
	uint64_t Numerator = (uint64_t)Map * 100 * Ve * FUEL_CONST; 
	uint32_t Denominator = (uint32_t)Afr * (Temperature + 273);  
	uint64_t InjectorTime = Numerator / Denominator;
	
	return (uint32_t)InjectorTime;
}