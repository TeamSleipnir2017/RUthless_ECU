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
	uint16_t Ve = math_interpolation_array(Rpm, Map, &VE, 1);
	engine_realtime.VeTarget = Ve;
	//uart_print_string("Ve: "); uart_print_int(Ve); uart_new_line();
	uint16_t Afr = math_interpolation_array(Rpm, Map, &AFR, 1);
	engine_realtime.AfrTarget = Afr;
	//uart_print_string("Afr: "); uart_print_int(Afr); uart_new_line();
	// Fuel equation:
	// Mair = (VE *  MAP * CYL_DISP) / (R (287 J/(Kg Kelvin)) * (IAT + 273))

	// MAP is in kPa, required to change it to Pa (1000) AND AFR is * 10, 13.0 = 130 AND VE is /100 since it is in % 
	// InjectorTime is in 100 nano seconds, InjectorTime = 10 = 1µs
	uint64_t Numerator = (uint64_t)Map * Ve * FUEL_CONST; 
	uint32_t Denominator = (uint32_t)Afr * (Temperature + 273);  
	uint64_t InjectorTime = Numerator / Denominator;

	// Calculate enrichments
	uint16_t gammaEnrich = fuelcalc_GammaEnrich();
	InjectorTime = (InjectorTime * gammaEnrich) /100;
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
	uint16_t TotalEnrich = 100;
	uint8_t CoolantTemperature = engine_realtime.Clt;
	// Calculate warm up enrichment
	TotalEnrich *= math_interpolation_vector(&engine_config2.WarmUpEnrichTemp ,&engine_config2.WarmUpEnrichPct, CoolantTemperature, 100, WARMUP_ENRICH_SIZE);
	TotalEnrich /= 100;

	// Calculate after start enrichment
	// TODO: Make more efficient way to add after start enrichment
	if (millis/MILLI_SEC < engine_config2.AfterStartEnrichSec)
	{
		TotalEnrich *= (100 + engine_config2.AfterStartEnrichPct);
		TotalEnrich /= 100;
	}

	// Calculate cranking enrichment
	if (engine_config2.CrankingRpm * RPM_SCALER > engine_realtime.Rpm)
	{
		LastCrankingCycle = CrankCycleCounter;
		TotalEnrich *= (100 + engine_config2.CrankingEnrichPct);
		TotalEnrich /= 100;
	}

	// Prevent overflow
	if (TotalEnrich > 255)
	{
		TotalEnrich = 255;
	}
	
	engine_realtime.GammaEnrich = TotalEnrich;
	return TotalEnrich;
}