/*
 * decoders.c
 *
 * Created: 12.3.2017 18:24:44
 *  Author: rikardur
 */ 


#include "decoders.h"


void decoders_crank_primary(void)
{	
	if(isDebug)
	{
		debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "CrankTooth", CrankTooth);
		debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "CrankToothCounter", CrankToothCounter);
		debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "CrankCycleCounter", CrankCycleCounter);
	}
	if(CrankCurrCycleCounts > (3 * (CrankPrevCycleCounts >> 1))) // New Cycle event (Missing tooth have passed the sensor)
	{
		CrankNewCycleFlag = TRUE;
		debug_cylinder[0].RealTimeCycleNr++;
		debug_cylinder[1].RealTimeCycleNr++;
		debug_cylinder[2].RealTimeCycleNr++;
		debug_cylinder[3].RealTimeCycleNr++;
// 			if (isDebug)
// 			{
// 				uart_print_string("C "); uart_print_int(CrankTooth); uart_new_line();
// 			}
		CrankTooth = 0;
		CrankCycleCounter++; 
			
		CamSignalFlag ^= TRUE;
/*
		IgnitionDegree = math_interpolation_array(engine_realtime.Rpm, engine_realtime.Map, &IGN, 1);
	
		DwellDegree = IgnitionDegree + igncalc_dwell_degree();
			
		DwellSecondTach = igncalc_ign_time_teeth(DwellDegree);
		DwellSecondInterval = igncalc_ign_time_interval(DwellDegree) + decoders_tooth_degree_correction();
			
		CrankSecondTach = igncalc_ign_time_teeth(IgnitionDegree);
		CrankSecondInterval = igncalc_ign_time_interval(IgnitionDegree) + decoders_tooth_degree_correction();
*/
		// RPM calculations
		uint32_t CalcRpm = GLOBAL_TIMER_FREQ * 60 / CrankRevCounts;
		// TODO: CHECK if calculated RPM is crap, well above redline (high frequency filter)
		engine_realtime.Rpm = (uint16_t)CalcRpm;
			
		LastCrankRevCounts = CrankRevCounts;
		CrankRevCounts = 0;
		if(isDebug)
		{
			debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "LastCrankRevCounts", LastCrankRevCounts);
		}
			
	}
	// TODO: DISABLE INTERRUPTS !!!!!!!!!!!!!!!!!!!!!
	uint8_t tempCrankTooth = CrankTooth;
	uint32_t tempCrankToothCounter = CrankToothCounter;
	// TODO: ENABLE INTERRUPTS !!!!!!!!!!!!!!!!!!!!!
		
	// TODO: WHAT IF THE MAIN FUNCTION MISSED A CRANK SIGNAL EVENT !!!!!!!!!!!!!!!!!
	int16_t tempCalc = (tempCrankTooth - TachEventDelayTeeths);
	if (tempCalc % TachPulse == 0) // TODO:  Counteract if the tach event is at the missing tooth
	{
		if(isDebug)
		{
			debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "TachTooth", CrankTooth);
		}
		decoders_tach_event(tempCrankTooth, tempCrankToothCounter);
		// global_toggle_pin(PIOC, IGN5_OUT); // used for debugging purposes
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
}

// The idea is to look two cycles beforhand for each cylinder
void decoders_tach_event(uint8_t CurrentCrankTooth, uint32_t CurrentCrankToothCounter)
{
	// Choose a cylinder to configure
	uint8_t NrCylinderDividedByTwo =  engine_config2.NrCylinders / 2;
	uint32_t TachEventNumber = (CurrentCrankTooth - TachEventDelayTeeths) / TachPulse; // Represents count of tach event in one revolution 
	uint32_t CylinderOffset = (engine_config4.SecondTriggerPolar ^ CamSignalFlag) * (NrCylinderDividedByTwo); // Represent first bank or second bank
	uint32_t InjIndex = TachEventNumber + CylinderOffset;						// Represents current cylinder tdc or 720°
	uint32_t IgnIndex = (TachEventNumber + CylinderOffset + NrCylinderDividedByTwo) % (engine_config2.NrCylinders); // Calculate cylinder after 360° 
	// TODO: CHOOSE CYLINDER ACCORDING TO FIRING ORDER
	struct cylinder_ *InjCylEvent = &cylinder[engine_config2.FiringOrder[InjIndex]]; // Next cylinder to calculate injection parameters for (720° from current position)
	struct cylinder_ *IgnCylEvent = &cylinder[engine_config2.FiringOrder[IgnIndex]]; // Next cylinder to calculate ignition parameters for (360° from current position) TODO: Check if dwell time is longer than max rpm time then calculate 720° before
	
	if (isDebug)
	{
		uart_debug_transfer_new_message(TC2->TC_CHANNEL[2].TC_CV, "InjCylEvent", engine_config2.FiringOrder[InjIndex]);
		DebugCounter++;
		if (DebugCounter == 101)
		{
// 		uart_print_string("TachPulse: "); uart_print_int(TachPulse); uart_new_line();
//  		uart_print_string("CurrentCrankTooth: "); uart_print_int(CurrentCrankTooth); uart_new_line();
// 		uart_print_string("CurrentCrankToothCounter: "); uart_print_int(CurrentCrankToothCounter); uart_new_line(); 
// 		uart_print_string("TachEventDelayTeeths: "); uart_print_int(TachEventDelayTeeths); uart_new_line();
// 		uart_print_string("TachEventNumber: "); uart_print_int(TachEventNumber); uart_new_line();
// 		uart_print_string("CylinderOffset: "); uart_print_int(CylinderOffset); uart_new_line();
// 		uart_print_string("InjIndex: "); uart_print_int(InjIndex); uart_new_line();
// 		uart_print_string("IgnIndex: "); uart_print_int(IgnIndex); uart_new_line();
 		}
	}
	// TODO: INCREASE DEGREE RESOLUTION TO 360.00° = 36000 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Injection timing calculations, OFF means turn output off and ON is output on
	uint32_t PulseWidth = fuelcalc_pulsewidth(); // Hundreds of nanoseconds (1 = 0.1 µs)
	engine_realtime.PulseWidth = PulseWidth / 1000; // convert to tenths of millisecs
	if ((PulseWidth > 0) || (engine_realtime.Rpm < (engine_config4.HardRevLimit * RPM_SCALER))) // If Flood clear OR rev limit
	{
		// Degrees are calculated from current crank position(some cylinder TDC), for example 390.0° is one cycle beforehand and 330.0° before next TDC
		uint32_t InjDegOFF = engine_config2.InjAng[InjIndex] * 10 + CRANK_DEGREE_RESOLUTION; // configured injector closing angle
		decoders_set_inj_or_ign_event(CurrentCrankTooth, CurrentCrankToothCounter, &InjCylEvent->Inj, PulseWidth, InjDegOFF);	
	}
	
	if (isDebug)
	{
		debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "IgnCylEvent", engine_config2.FiringOrder[IgnIndex]);
	}

	// Ignition timing calculations
	uint32_t DwellPulseWidth = igncalc_dwell_pulsewidth();
	uint16_t DegreeAdvance = math_interpolation_array(engine_realtime.Rpm, engine_realtime.Map, &IGN, 1);
	engine_realtime.DegAdvance = (uint8_t) (DegreeAdvance / 10);
	if (engine_realtime.Rpm < (engine_config4.SoftRevLimit * RPM_SCALER))
	{
		uint32_t IgnDegOFF = CRANK_DEGREE_RESOLUTION - DegreeAdvance;
		decoders_set_inj_or_ign_event(CurrentCrankTooth, CurrentCrankToothCounter, &IgnCylEvent->Ign, DwellPulseWidth, IgnDegOFF);
	}
	
// 	int16_t InjDegON = InjDegOFF - math_convert_pulsewidth_to_crank_degrees(PulseWidth);
// 	if (InjDegON < 0) // If perhaps the calculated pulsewidth is longer than 2 crank cycles (duty cycle > 100%)
// 	{
// 		InjDegON = 0;
// 		// TODO: TURN THE INJECTOR ON
// 		// PROBABLY SET SOME FLAG OR SOME
// 	}
// 	
// 	// The number of teeths are calculated
// 	uint16_t InjTeethsOFF = math_convert_degree_to_teeth_count(InjDegOFF); // OFF means when to turn off the injector
// 	uint16_t InjTeethsON = math_convert_degree_to_teeth_count(InjDegON); // ON means when to turn on the injector
// 	//uint16_t InjTeethsON = InjTeethsOFF - (math_convert_pulsewidth_to_teeth_count(PulseWidth + InjectorOpenTime) + 1); // 1 is because it floors the calculation and it is better to get always scaled one up since it is also timer dependant
// 	
// 	// The event tooth is found according to the delay teeths calculated above, here are missing tooth counts skipped and taken into account for timers
// 	uint32_t InjEventToothOFF = CurrentCrankToothCounter, InjEventToothON = CurrentCrankToothCounter; // Copying values to different register, to be able to manipulate the values without changing CurrentCrankToothCounter
// 	uint32_t NrOfMissingTeethsAtEventOFF = math_find_event_tooth_from_number_of_teeths(CurrentCrankTooth, &InjEventToothOFF, InjTeethsOFF);
// 	uint32_t NrOfMissingTeethsAtEventON = math_find_event_tooth_from_number_of_teeths(CurrentCrankTooth, &InjEventToothON, InjTeethsON);
// 	
// 	// Load the event tooth to the according cylinder struct
// 	InjCylEvent->Inj.ToothOff = InjEventToothOFF;
// 	InjCylEvent->Inj.ToothOn = InjEventToothON;
// 	
// 	if (InjCylEvent->Inj.ToothOn == InjCylEvent->Inj.ToothOff)
// 	{
// 		// RAISE A FLAG WHICH IS USED IN TIMER INTERRUPT TO LOAD RB THE InjCntTimingOff
// 		InjCylEvent->Inj.EventOnSameTooth = TRUE;
// 	}
// 	
// 	// The number of counts for the timer is calculated here (which is enabled after the EventTooth)
// 	// If the event is at the last tooth before missing tooth, and if it is applicable it is added to the counts of the timer
// 	uint32_t TimerTurnOffDegreeAfterTooth = InjDegOFF % CrankToothDegreeInterval; 
// 	uint32_t TimerToothOffPercentage = TimerTurnOffDegreeAfterTooth * 100 / CrankToothDegreeInterval; // should give 0 - 99%, since 100% is next tooth obviously
// 	InjCylEvent->Inj.CntTimingOff = ((TimerToothOffPercentage + NrOfMissingTeethsAtEventOFF * 100) * LastCrankRevCounts) / (100 * engine_config4.TriggerTeethCount); // I think this should never overflow
// 	
// 	uint32_t TimerTurnOnDegreeAfterTooth = InjDegON % CrankToothDegreeInterval;
// 	uint32_t TimerToothOnPercentage = TimerTurnOnDegreeAfterTooth * 100 / CrankToothDegreeInterval; // should give 0 - 99%, since 100% is next tooth obviously
// 	InjCylEvent->Inj.CntTimingOn = ((TimerToothOnPercentage + NrOfMissingTeethsAtEventON * 100) * LastCrankRevCounts) / (100 * engine_config4.TriggerTeethCount); // I think this should never overflow
// 
// 	if (isDebug)
// 	{
// 		if (DebugCounter == 101)
// 		{
// // 		uart_print_string("PulseWidth: "); uart_print_int(PulseWidth); uart_new_line();
// // 		uart_print_string("InjDeg: "); uart_print_int(InjDeg); uart_new_line();
// // 		uart_print_string("InjTeethsOFF: "); uart_print_int(InjTeethsOFF); uart_new_line();
// // 		uart_print_string("InjTeethsON: "); uart_print_int(InjTeethsON); uart_new_line();
// // 		uart_print_string("CurrentCrankToothCounter: "); uart_print_int(CurrentCrankToothCounter); uart_new_line();
// // 		uart_print_string("InjEventToothOFF: "); uart_print_int(InjEventToothOFF); uart_new_line();
// // 		uart_print_string("InjEventToothON: "); uart_print_int(InjEventToothON); uart_new_line();
// // 		uart_print_string("NrOfMissingTeethsAtEventOFF: "); uart_print_int(NrOfMissingTeethsAtEventOFF); uart_new_line();
// // 		uart_print_string("NrOfMissingTeethsAtEventON: "); uart_print_int(NrOfMissingTeethsAtEventON); uart_new_line();
// // 		uart_print_string("cylinder[InjIndex].InjToothOff: "); uart_print_int(cylinder[InjIndex].InjToothOff); uart_new_line();
// // 		uart_print_string("cylinder[InjIndex].InjToothOn: "); uart_print_int(cylinder[InjIndex].InjToothOn); uart_new_line();
//  		DebugCounter = 0;
// 		}
// 		debug_cylinder[InjIndex].InjTargetTurnOffDegree = InjDegOFF;
// 		debug_cylinder[InjIndex].InjTargetPulseWidth = PulseWidth;
// 		debug_cylinder[InjIndex].InjRealTimeCalcCount = InjCylEvent->Tc_channel->TC_CV;
// 	}

	

}

void decoders_set_inj_or_ign_event(uint8_t CurrentCrankTooth, uint32_t CurrentCrankToothCounter, struct cylinder_output_manager *Inj_or_Ign, uint32_t PulseWidth, uint16_t DegreeOff)
{
	int16_t DegreeOn = DegreeOff - math_convert_pulsewidth_to_crank_degrees(PulseWidth);
	if (DegreeOn < 0) // If perhaps the calculated pulsewidth is longer than 2 crank cycles (duty cycle > 100%)
	{
		DegreeOn = 0;
		// TODO: TURN THE INJECTOR/IGNITION ON
		// PROBABLY SET SOME FLAG OR SOME
	}
	
	// The number of teeths are calculated
	uint16_t TeethsOFF = math_convert_degree_to_teeth_count(DegreeOff); // OFF means when to turn off the injector
	uint16_t TeethsON = math_convert_degree_to_teeth_count(DegreeOn); // ON means when to turn on the injector
	//uint16_t InjTeethsON = InjTeethsOFF - (math_convert_pulsewidth_to_teeth_count(PulseWidth + InjectorOpenTime) + 1); // 1 is because it floors the calculation and it is better to get always scaled one up since it is also timer dependant
	
	// The event tooth is found according to the delay teeths calculated above, here are missing tooth counts skipped and taken into account for timers
	uint32_t EventToothOFF = CurrentCrankToothCounter, EventToothON = CurrentCrankToothCounter; // Copying values to different register, to be able to manipulate the values without changing CurrentCrankToothCounter
	uint32_t NrOfMissingTeethsAtEventOFF = math_find_event_tooth_from_number_of_teeths(CurrentCrankTooth, &EventToothOFF, TeethsOFF);
	uint32_t NrOfMissingTeethsAtEventON = math_find_event_tooth_from_number_of_teeths(CurrentCrankTooth, &EventToothON, TeethsON);
	
	// Load the event tooth to the according cylinder struct
	Inj_or_Ign->ToothOff = EventToothOFF;
	Inj_or_Ign->ToothOn = EventToothON;
	
	if (Inj_or_Ign->ToothOn == Inj_or_Ign->ToothOff)
	{
		// RAISE A FLAG WHICH IS USED IN TIMER INTERRUPT TO LOAD RB THE CntTimingOff
		Inj_or_Ign->EventOnSameTooth = TRUE;
	}
	
	// The number of counts for the timer is calculated here (which is enabled after the EventTooth)
	// If the event is at the last tooth before missing tooth, and if it is applicable it is added to the counts of the timer
	uint32_t TimerTurnOffDegreeAfterTooth = DegreeOff % CrankToothDegreeInterval;
	uint32_t TimerToothOffPercentage = TimerTurnOffDegreeAfterTooth * 100 / CrankToothDegreeInterval; // should give 0 - 99%, since 100% is next tooth obviously
	Inj_or_Ign->CntTimingOff = ((TimerToothOffPercentage + NrOfMissingTeethsAtEventOFF * 100) * LastCrankRevCounts) / (100 * engine_config4.TriggerTeethCount); // I think this should never overflow
	
	uint32_t TimerTurnOnDegreeAfterTooth = DegreeOn % CrankToothDegreeInterval;
	uint32_t TimerToothOnPercentage = TimerTurnOnDegreeAfterTooth * 100 / CrankToothDegreeInterval; // should give 0 - 99%, since 100% is next tooth obviously
	Inj_or_Ign->CntTimingOn = ((TimerToothOnPercentage + NrOfMissingTeethsAtEventON * 100) * LastCrankRevCounts) / (100 * engine_config4.TriggerTeethCount); // I think this should never overflow

	if (isDebug)
	{
		debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "CurrentCrankcnt", CurrentCrankToothCounter);
		debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "PulseWidth", PulseWidth);
		debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "DegreeOn", DegreeOn);
		debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "DegreeOff", DegreeOff);
		debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "TeethsOFF", TeethsOFF);
		debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "TeethsON", TeethsON);
		debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "NrOfMissingOFF", NrOfMissingTeethsAtEventOFF);
		debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "NrOfMissingON", NrOfMissingTeethsAtEventON);
		debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "EventToothOFF", EventToothOFF);
		debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "EventToothON", EventToothON);
		debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "CrankInterval", CurrentCrankToothCounter);
		debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "TimerTurnOff", TimerTurnOffDegreeAfterTooth);
		debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "TimerPerc", TimerToothOffPercentage);
		debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "CntTimingOff", Inj_or_Ign->CntTimingOff);
		debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "TimerTurnOn", TimerTurnOnDegreeAfterTooth);
		debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "TimerOnPerc", TimerToothOnPercentage);
		debug_transfer_new_message(&myDebug, TC2->TC_CHANNEL[2].TC_CV, "CntTimingOn", Inj_or_Ign->CntTimingOn);
		if (DebugCounter == 101)
		{
		
// 		uart_print_string("CurrentCrankcnt: "); uart_print_int(CurrentCrankToothCounter); uart_new_line();
// 			
// 		uart_print_string("PulseWidth: "); uart_print_int(PulseWidth); uart_new_line();
// 		uart_print_string("DegreeOn: "); uart_print_int(DegreeOn); uart_new_line();
// 		uart_print_string("DegreeOff: "); uart_print_int(DegreeOff); uart_new_line();
// 		uart_print_string("TeethsOFF: "); uart_print_int(TeethsOFF); uart_new_line();
// 		uart_print_string("TeethsON: "); uart_print_int(TeethsON); uart_new_line();
// 		
// 		uart_print_string("NrOfMissingOFF: "); uart_print_int(NrOfMissingTeethsAtEventOFF); uart_new_line();
// 		uart_print_string("NrOfMissingON: "); uart_print_int(NrOfMissingTeethsAtEventON); uart_new_line();
// 		uart_print_string("EventToothOFF: "); uart_print_int(EventToothOFF); uart_new_line();
// 		uart_print_string("EventToothON: "); uart_print_int(EventToothON); uart_new_line();
// 		uart_print_string("CrankInterval: "); uart_print_int(CrankToothDegreeInterval); uart_new_line();
// 		
// 		
// 		uart_print_string("TimerTurnOff: "); uart_print_int(TimerTurnOffDegreeAfterTooth); uart_new_line();
// 		uart_print_string("TimerPerc: "); uart_print_int(TimerToothOffPercentage); uart_new_line();
// 		uart_print_string("CntTimingOff: "); uart_print_int(Inj_or_Ign->CntTimingOff); uart_new_line();
// 		uart_print_string("TimerTurnOn: "); uart_print_int(TimerTurnOnDegreeAfterTooth); uart_new_line();
// 		uart_print_string("TimerOnPerc: "); uart_print_int(TimerToothOnPercentage); uart_new_line();
// 		uart_print_string("CntTimingOn: "); uart_print_int(Inj_or_Ign->CntTimingOn); uart_new_line();
		
 		DebugCounter = 0;
		}
	}
}



// CURRENTLY NOT USED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 16.4.17
// returns teeth numbers to next event
uint32_t decoders_find_event_tooth(uint32_t CrankDegree, uint32_t CurrentCrankToothCounter)
{
	uint16_t InjTeeths = math_convert_degree_to_teeth_count(CrankDegree); 
	
}

// Ignition timing correction.  
// The 0° is at the middle of the tooth and calculations are done at the edge of every tooth.
// This function corrects the calculations by shifting the tach degree (0° and 180° in this case) to the middle of the tooth
uint32_t decoders_tooth_degree_correction(void)
{
	return CrankPrevCycleCounts/4;
}

