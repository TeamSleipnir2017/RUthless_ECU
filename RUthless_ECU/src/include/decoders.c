/*
 * decoders.c
 *
 * Created: 12.3.2017 18:24:44
 *  Author: rikardur
 */ 


#include "decoders.h"


void decoders_crank_primary(void)
{
	if (CrankSignalFlag)
	{
		if(CrankCurrCycleCounts > (3 * (CrankPrevCycleCounts >> 1)))
		{
			if (isDebug)
			{
				uart_print_string("C "); uart_print_int(CrankTooth); uart_new_line();
			}
			CrankTooth = 0;
			CrankSecondTooth = TachPulse;
			
			// uart_print_string("C "); uart_print_int(CrankTooth); uart_new_line();
			CamSignalFlag ^= TRUE;
			IgnitionDegree = math_interpolation_array(engine_realtime.Rpm, engine_realtime.Map, &IGN);
			
			DwellDegree = IgnitionDegree + igncalc_dwell_degree();
			if (isDebug)
			{
				//uart_transfer('C'); uart_print_int(igncalc_dwell_degree()); uart_new_line();
			}
			
			// uart_transfer('D'); uart_print_int(DwellDegree); uart_new_line();
			DwellSecondTach = igncalc_ign_time_teeth(DwellDegree);
			DwellSecondInterval = igncalc_ign_time_interval(DwellDegree) + decoders_tooth_degree_correction();
			
			
			CrankSecondTach = igncalc_ign_time_teeth(IgnitionDegree);
			CrankSecondInterval = igncalc_ign_time_interval(IgnitionDegree) + decoders_tooth_degree_correction();
			
			/*
			// Dwell interval calculation, When to turn on ignition pins for first tach event
			DwellDegree = DEGREE_TEST - igncalc_dwell_time()/(CrankRevCounts/360);
			
			DwellFirstTach = 11 - igncalc_ign_time_teeth(DwellDegree);
			uart_transfer('a'); uart_print_int(DwellDegree); uart_new_line();
			DwellFirstInterval = igncalc_ign_time_interval(DwellDegree) + decoders_tooth_degree_correction();
			//uart_transfer('e'); uart_print_int(DwellFirstInterval); uart_new_line();
			
			// Ignition interval calculation, When to turn off ignition pins for first tach event
			CrankFirstTach = 11 - igncalc_ign_time_teeth(DEGREE_TEST);
			CrankFirstInterval = igncalc_ign_time_interval(DEGREE_TEST) + decoders_tooth_degree_correction();
			*/
			/*
			if (DwellFirstTach == CrankFirstTach)
			{
				SameFirstTooth = TRUE;
				CrankFirstInterval = CrankFirstInterval - DwellFirstInterval;
			}
			*/
			
			
			// RPM calculations
			uint64_t CalcRpm = GLOBAL_TIMER_FREQ * 60 / CrankRevCounts;
			// TODO: CHECK if calculated RPM is crap, well above redline (high frequency filter)
			engine_realtime.Rpm = (uint16_t)CalcRpm;
			
			
			
			CrankRevCounts = 0;
			
		}
		else if (CrankTooth == TachPulse)
		{
			CrankSecondTooth = 0;
			IgnitionDegree = math_interpolation_array(engine_realtime.Rpm, engine_realtime.Map, &IGN);
			
			DwellFirstTach = igncalc_ign_time_teeth(DwellDegree);
			/*if (DwellFirstTach < TachPulse && DwellFirstTach > TachPulse - )
			{
			}*/
			DwellFirstInterval = igncalc_ign_time_interval(DwellDegree) + decoders_tooth_degree_correction();
			
			CrankFirstTach = igncalc_ign_time_teeth(IgnitionDegree);
			CrankFirstInterval = igncalc_ign_time_interval(IgnitionDegree) + decoders_tooth_degree_correction();
			
			/*
			// Dwell interval calculation, When to turn on ignition pins for second tach event
			DwellSecondTach = 23 - igncalc_ign_time_teeth(DwellDegree);
			DwellSecondInterval = igncalc_ign_time_interval(DwellDegree) + decoders_tooth_degree_correction();
			uart_transfer('e'); uart_print_int(DwellSecondInterval); uart_new_line();
			
			// Ignition interval calculation, When to turn off ignition pins for second tach event
			CrankSecondTach = 23 - igncalc_ign_time_teeth(DEGREE_TEST);
			CrankSecondInterval = igncalc_ign_time_interval(DEGREE_TEST) + decoders_tooth_degree_correction();
			*/
			
		}
		CrankSignalFlag = FALSE;
		
	}
}

// Ignition timing correction.  
// The 0° is at the middle of the tooth and calculations are done at the edge of every tooth.
// This function corrects the calculations by shifting the tach degree (0° and 180° in this case) to the middle of the tooth
uint32_t decoders_tooth_degree_correction(void)
{
	return CrankPrevCycleCounts/4;
}




// Toggle ignition pin 1
void decoders_toggle_ign1pin(void)
{
	uint32_t status =	PIOC->PIO_ODSR;			// Store the status on pins in port C
	/*	Toggle output pin PC19	*/
	if (status & PIO_ODSR_P19)					// AND the status on pins in port C and the status of pin 19
	{
		PIOC->PIO_CODR	=	IGN1_OUT;			// If PC19 is high,	Clear Output Data Register	-	Sets pin PC19 to low
	}
	else
	{
		PIOC->PIO_SODR	=	IGN1_OUT;			// If PC19 is low,	Set Output Data Register	-	Sets pin PC19 to high
	}
}

// Toggle ignition pin 2
void decoders_toggle_ign2pin(void)
{
	uint32_t status =	PIOC->PIO_ODSR;			// Store the status on pins in port C
	/*	Toggle output pin PC19	*/
	if (status & PIO_ODSR_P23)					// AND the status on pins in port C and the status of pin 19
	{
		PIOC->PIO_CODR	=	IGN2_OUT;			// If PC19 is high,	Clear Output Data Register	-	Sets pin PC19 to low
	}
	else
	{
		PIOC->PIO_SODR	=	IGN2_OUT;			// If PC19 is low,	Set Output Data Register	-	Sets pin PC19 to high
	}
}

// Toggle ignition pin 3
void decoders_toggle_ign3pin(void)
{
	uint32_t status =	PIOC->PIO_ODSR;			// Store the status on pins in port C
	/*	Toggle output pin PC19	*/
	if (status & PIO_ODSR_P17)					// AND the status on pins in port C and the status of pin 19
	{
		PIOC->PIO_CODR	=	IGN3_OUT;			// If PC19 is high,	Clear Output Data Register	-	Sets pin PC19 to low
	}
	else
	{
		PIOC->PIO_SODR	=	IGN3_OUT;			// If PC19 is low,	Set Output Data Register	-	Sets pin PC19 to high
	}
}

// Toggle ignition pin 4
void decoders_toggle_ign4pin(void)
{
	uint32_t status =	PIOC->PIO_ODSR;			// Store the status on pins in port C
	/*	Toggle output pin PC19	*/
	if (status & PIO_ODSR_P25)					// AND the status on pins in port C and the status of pin 25
	{
		PIOC->PIO_CODR	=	IGN4_OUT;			// If PC25 is high,	Clear Output Data Register	-	Sets pin PC25 to low
	}
	else
	{
		PIOC->PIO_SODR	=	IGN4_OUT;			// If PC25 is low,	Set Output Data Register	-	Sets pin PC25 to high
	}
}