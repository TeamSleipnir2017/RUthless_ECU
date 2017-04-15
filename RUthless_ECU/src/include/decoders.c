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
		
		if(CrankCurrCycleCounts > (3 * (CrankPrevCycleCounts >> 1))) // New Cycle event (Missing tooth have passed the sensor)
		{
// 			if (isDebug)
// 			{
// 				uart_print_string("C "); uart_print_int(CrankTooth); uart_new_line();
// 			}
			CrankTooth = 0;
			
			CamSignalFlag ^= TRUE;
			IgnitionDegree = math_interpolation_array(engine_realtime.Rpm, engine_realtime.Map, &IGN, 1);

			
			DwellDegree = IgnitionDegree + igncalc_dwell_degree();

			
			DwellSecondTach = igncalc_ign_time_teeth(DwellDegree);
			DwellSecondInterval = igncalc_ign_time_interval(DwellDegree) + decoders_tooth_degree_correction();
			

			CrankSecondTach = igncalc_ign_time_teeth(IgnitionDegree);
			CrankSecondInterval = igncalc_ign_time_interval(IgnitionDegree) + decoders_tooth_degree_correction();

			
			
			// RPM calculations
			uint32_t CalcRpm = GLOBAL_TIMER_FREQ * 60 / CrankRevCounts;
			// TODO: CHECK if calculated RPM is crap, well above redline (high frequency filter)
			engine_realtime.Rpm = (uint16_t)CalcRpm;
			
			engine_realtime.PulseWidth = fuelcalc_pulsewidth() / 1000; // REMEMBER TO REMOVE !!!!!
			
			LastCrankRevCounts = CrankRevCounts;
			CrankRevCounts = 0;
			
		}
		// TODO: DISABLE INTERRUPTS !!!!!!!!!!!!!!!!!!!!!
		uint8_t tempCrankTooth = CrankTooth;
		uint32_t tempCrankToothCounter = CrankToothCounter;
		// TODO: ENABLE INTERRUPTS !!!!!!!!!!!!!!!!!!!!!
		
		int16_t tempCalc = (tempCrankTooth - TachEventDelayTeeths);
		if (tempCalc % TachPulse == 0) // TODO:  Counteract if the tach event is at the missing tooth
		{
			decoders_tach_event(tempCrankTooth, tempCrankToothCounter);
			global_toggle_pin(PIOC, IGN5_OUT);
		}
// 		else if (CrankTooth == TachPulse)
// 		{
// 			
// 			CrankSecondTooth = 0;
// 			IgnitionDegree = math_interpolation_array(engine_realtime.Rpm, engine_realtime.Map, &IGN);
// 			
// 			DwellFirstTach = igncalc_ign_time_teeth(DwellDegree);
// 			/*if (DwellFirstTach < TachPulse && DwellFirstTach > TachPulse - )
// 			{
// 			}*/
// 			DwellFirstInterval = igncalc_ign_time_interval(DwellDegree) + decoders_tooth_degree_correction();
// 			
// 			CrankFirstTach = igncalc_ign_time_teeth(IgnitionDegree);
// 			CrankFirstInterval = igncalc_ign_time_interval(IgnitionDegree) + decoders_tooth_degree_correction();		
// 		}
		CrankSignalFlag = FALSE;
		
	}
}

// The idea is to look two cycles beforhand for each cylinder
void decoders_tach_event(uint8_t CurrentCrankTooth, uint32_t CurrentCrankToothCounter)
{
	// Choose a cylinder to configure
	uint8_t NrCylinderDividedByTwo =  engine_config2.NrCylinders / 2;
	uint32_t TachEventNumber = (CurrentCrankTooth - TachEventDelayTeeths) / TachPulse; // Represents count of tach event in one revolution 
	uint32_t CylinderOffset = CamSignalFlag * (NrCylinderDividedByTwo); // Represent first bank or second bank
	uint32_t InjIndex = TachEventNumber + CylinderOffset;						// Represents current cylinder tdc or 720°
	uint32_t IgnIndex = (TachEventNumber + CylinderOffset + NrCylinderDividedByTwo) % (engine_config2.NrCylinders); // Calculate cylinder after 360° 
	// TODO: CHOOSE CYLINDER ACCORDING TO FIRING ORDER
	struct cylinder_ *InjCylEvent = &cylinder[InjIndex]; // Next cylinder to calculate injection parameters for (720° from current position)
	struct cylinder_ *IgnCylEvent = &cylinder[IgnIndex]; // Next cylinder to calculate ignition parameters for (360° from current position) TODO: Check if dwell time is longer than max rpm time then calculate 720° before
	
	if (isDebug)
	{
		DebugCounter++;
		if (DebugCounter == 101)
		{
// 		uart_print_string("TachPulse: "); uart_print_int(TachPulse); uart_new_line();
 		uart_print_string("CurrentCrankTooth: "); uart_print_int(CurrentCrankTooth); uart_new_line();
// 		uart_print_string("TachEventDelayTeeths: "); uart_print_int(TachEventDelayTeeths); uart_new_line();
// 		uart_print_string("TachEventNumber: "); uart_print_int(TachEventNumber); uart_new_line();
// 		uart_print_string("CylinderOffset: "); uart_print_int(CylinderOffset); uart_new_line();
// 		uart_print_string("InjIndex: "); uart_print_int(InjIndex); uart_new_line();
// 		uart_print_string("IgnIndex: "); uart_print_int(IgnIndex); uart_new_line();
 		}
	}

	// Injection timing calculations
	uint32_t PulseWidth = fuelcalc_pulsewidth(); // Hundreds of nanoseconds (1 = 0.1 µs)
	uint16_t InjDeg = engine_config2.InjAng[InjIndex]; // configured injector closing angle
	// The number of teeths are calculated from now
	uint16_t InjTeethsOFF = math_convert_degree_to_teeth_count(InjDeg + 360); // OFF means when to turn off the injector, 360 is because it is 720° before
	uint16_t InjTeethsON = InjTeethsOFF - (math_convert_pulsewidth_to_teeth_count(PulseWidth + (engine_config2.injOpen * 1000)) + 1); // 1 is because it floors the calculation and it is better to get always scaled one up since it is also timer dependant
	uint32_t InjEventToothOFF = math_find_event_tooth_from_number_of_teeths(CurrentCrankTooth, CurrentCrankToothCounter, InjTeethsOFF);
	uint32_t InjEventToothON = math_find_event_tooth_from_number_of_teeths(CurrentCrankTooth, CurrentCrankToothCounter, InjTeethsON);
	InjCylEvent->InjToothOff = InjEventToothOFF;
	InjCylEvent->InjToothOn = InjEventToothON;

	if (isDebug)
	{
		if (DebugCounter == 101)
		{
// 		uart_print_string("PulseWidth: "); uart_print_int(PulseWidth); uart_new_line();
// 		uart_print_string("InjDeg: "); uart_print_int(InjDeg); uart_new_line();
// 		uart_print_string("InjTeethsOFF: "); uart_print_int(InjTeethsOFF); uart_new_line();
// 		uart_print_string("InjTeethsON: "); uart_print_int(InjTeethsON); uart_new_line();
// 		uart_print_string("CurrentCrankToothCounter: "); uart_print_int(CurrentCrankToothCounter); uart_new_line();
// 		uart_print_string("InjEventToothOFF: "); uart_print_int(InjEventToothOFF); uart_new_line();
// 		uart_print_string("InjEventToothON: "); uart_print_int(InjEventToothON); uart_new_line();
// 		uart_print_string("cylinder[InjIndex].InjToothOff: "); uart_print_int(cylinder[InjIndex].InjToothOff); uart_new_line();
// 		uart_print_string("cylinder[InjIndex].InjToothOn: "); uart_print_int(cylinder[InjIndex].InjToothOn); uart_new_line();
 		DebugCounter = 0;
		}
	}



	





	// Ignition timing calculations
	uint32_t IgnDeg = math_interpolation_array(engine_realtime.Rpm, engine_realtime.Map, &IGN, 10);
	// Calculate TEETHS for IgnDeg
	// Calculate DELAY COUNTS for IgnDeg
	// Calculate TEETHS from IgnDeg time to Dwell time
	// Calculate DELAY COUNTS from IgnDeg 
	//uint32_t DwellDeg = IgnDeg + igncalc_dwell_degree(); // This is probably just to make it harder and longer to calculate, convert from time to degrees and there to counts

	

// 	DwellSecondTach = igncalc_ign_time_teeth(DwellDegree);
// 	DwellSecondInterval = igncalc_ign_time_interval(DwellDegree) + decoders_tooth_degree_correction();
// 	
// 	CrankSecondTach = igncalc_ign_time_teeth(IgnitionDegree);
// 	CrankSecondInterval = igncalc_ign_time_interval(IgnitionDegree) + decoders_tooth_degree_correction();
	// Fuel timing calculations
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