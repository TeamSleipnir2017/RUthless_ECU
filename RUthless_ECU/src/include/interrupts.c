/*
 * interrupts.c
 *
 * Created: 7.12.2016 14:31:53
 *  Author: jbbja
 */ 

#include "interrupts.h"

/* Initialize nested vector interrupts */
void interrupts_enable_interrupt_vector(uint32_t IRQN, uint32_t Priority)
{
	NVIC_DisableIRQ(IRQN);
	NVIC_ClearPendingIRQ(IRQN);
	NVIC_SetPriority(IRQN, Priority); 
	NVIC_EnableIRQ(IRQN);
}


void interrupts_enable_pio(uint32_t PeripheralID, uint32_t Pin, uint32_t Priority, uint8_t Mode)
{
	// Check if the programmer is legit
	if (PeripheralID < ID_PIOA || PeripheralID > ID_PIOD)	// PIOD is the last register in SAM3x8e
		return;
	
	// Enable interrupt registers 
	PMC->PMC_PCER0	=	(1 << PeripheralID);				// Enable the peripheral clock for port A
	
	// Choose the correct peripheral controller
	Pio *CurrentPio;
	switch (PeripheralID)
	{
		case ID_PIOA:
			CurrentPio = PIOA;
			break;
		case ID_PIOB:
			CurrentPio = PIOB;
			break;
		case ID_PIOC:
			CurrentPio = PIOC;
			break;
		case ID_PIOD:
			CurrentPio = PIOD;
			break;
		default: // SHOULD NEVER HAPPEN, because of if function above
			break;
	}
	CurrentPio->PIO_IER	=	Pin;										// Enable the interrupt register on the configured pin
	interrupts_enable_interrupt_vector(PeripheralID, Priority);		// Enable nested interrupt vector and set priority for the configured controller
	
	// Select the interrupt mode, if the programmer is stupid the default mode will be pin change
	switch (Mode)
	{
		case INTERRUPT_PIN_CHANGE_MODE:
			break; // No need to configure anything else, just for show
		case INTERRUPT_RISING_EDGE_MODE:
			CurrentPio->PIO_AIMER		=	Pin;				// Enable additional interrupt modes
			CurrentPio->PIO_ESR			=	Pin;				// Select edge detection
			CurrentPio->PIO_REHLSR		=	Pin;				// Select rising edge detection
			break;
		case INTERRUPT_FALLING_EDGE_MODE:
			CurrentPio->PIO_AIMER		=	Pin;				// Enable additional interrupt modes
			CurrentPio->PIO_ESR			=	Pin;				// Select edge detection
			CurrentPio->PIO_FELLSR		=	Pin;				// Select falling edge detection
			break;
		default:
			break;
	}
	// TODO: Decide if there is need of debouncing hardware filter	
	/*	Debouncing filter for a mechanical switch on pin D31	*/
	// PIOA->PIO_IFER	=	PIO_PA15;				// Enable input glitch filter register
	// PIOA->PIO_DIFSR	=	PIO_PA15;				// Select debouncing filter
	// PIOA->PIO_SCDR	=	0x01;
}


// Interrupt handler for the crankshaft and camshaft signal
void PIOA_Handler(void)
{
	uint32_t TimerCounterValue	=		TC2->TC_CHANNEL[2].TC_CV;
	uint32_t status_register	=		PIOA->PIO_ISR;
	
	// Check if the interrupt source is from the crankshaft sensor
	if (status_register & CRANK_SIGNAL)
	{
		// Start to filter out short pulses
		uint32_t tempGapTimeBetweenTeeths = TimerCounterValue - CrankTimerCounts;
		if (tempGapTimeBetweenTeeths >= TriggerFilterTime) // If pulses are less than filter time it means a false trigger
		{
			CrankPrevCycleCounts	=		CrankCurrCycleCounts;
			CrankCurrCycleCounts	=		TimerCounterValue - CrankTimerCounts;
			CrankTimerCounts		=		TimerCounterValue;
			CrankRevCounts			+=		CrankCurrCycleCounts;
			CrankTooth++;
			CrankToothCounter++;
			CrankSignalFlag			=		TRUE;
			for (uint8_t i = 0; i < engine_config2.NrCylinders; i++)
			{
				/*if(isDebug)
				{
					debug_transfer_new_message(&myDebug, TimerCounterValue, "CylInj", i);	
				}*/
				interrupts_check_timer_for_inj_or_ign(&cylinder[i].Inj, &cylinder[i], &debug_cylinders.inj_output_debug[i]);
				/*if(isDebug)
				{
					debug_transfer_new_message(&myDebug, TimerCounterValue, "CylIgn", i);	
				}*/
				interrupts_check_timer_for_inj_or_ign(&cylinder[i].Ign, &cylinder[i], &debug_cylinders.ign_output_debug[i]);
			}
			debug_cylinders.RealTimeCrank[debug_cylinders.RealTimeCrankCnt] = TimerCounterValue;
			debug_cylinders.RealTimeCrankCnt = (debug_cylinders.RealTimeCrankCnt + 1) % 50;
		}
		
		/*if(isDebug)
		{
			debug_transfer_new_message(&myDebug, TimerCounterValue, "Crank", 0);	
		}*/
		
	}
	
	// Check if the interrupt source is from the camshaft sensor
	if (status_register & CAM_SIGNAL)
	{
		/*if(isDebug)
		{
			debug_transfer_new_message(&myDebug, TimerCounterValue, "Cam", 0);	
		}*/
		CamCurrCycleCounts		=		TimerCounterValue - CamTimerCounts;
		CamTimerCounts			=		TimerCounterValue;
		CamSignalFlag			=		TRUE;
		
		debug_cylinders.RealTimeCam[debug_cylinders.RealTimeCamCnt] = TimerCounterValue;
		debug_cylinders.RealTimeCamCnt = (debug_cylinders.RealTimeCamCnt + 1) % 2;
	}
	
	PIOAHandlerTimeInCounts = TC2->TC_CHANNEL[2].TC_CV - TimerCounterValue;
}

void interrupts_check_timer_for_inj_or_ign(struct cylinder_output_manager *Inj_or_Ign, struct cylinder_ *Cyl, struct debug_cylinder_output_ *Debug_Output)
{
	if (Inj_or_Ign->ToothOn == CrankToothCounter) //turn output ON
	{
		if (Inj_or_Ign->CntTimingOn < PIOA_HANDLER_TIME_IN_COUNTS) // The interrupt vector does not react when a value is stored in compare register which has been reached in the end of this function/handler
		{
			/*if(isDebug)
			{
				debug_transfer_new_message(&myDebug, Cyl->Tc_channel->TC_CV, "On", Inj_or_Ign->CntTimingOn);
			}*/
			Inj_or_Ign->pio->PIO_CODR = Inj_or_Ign->OutputPin;			// Sets pin to high
			Debug_Output->RealTimeTurnOnCount = Cyl->Tc_channel->TC_CV;
			if (Inj_or_Ign->EventOnSameTooth)							// Check if Off event is at the same tooth
			{
				*(Inj_or_Ign->TcCompareRegister) = math_sum_with_overflow_protection(Cyl->Tc_channel->TC_CV, Inj_or_Ign->CntTimingOff - Inj_or_Ign->CntTimingOn);
				Inj_or_Ign->EventOnSameTooth = FALSE;
				/*if(isDebug)
				{
					debug_transfer_new_message(&myDebug, Cyl->Tc_channel->TC_CV, "SameTooth", *(Inj_or_Ign->TcCompareRegister));	
				}*/
			}
			else // Initiate output timeout
			{
				*(Inj_or_Ign->TcCompareRegister) = math_sum_with_overflow_protection(Cyl->Tc_channel->TC_CV, Inj_or_Ign->CntTimeOutOff);
			}
		}
		else
		{
			Inj_or_Ign->EventPending = TRUE;
			*(Inj_or_Ign->TcCompareRegister) = math_sum_with_overflow_protection(Cyl->Tc_channel->TC_CV, Inj_or_Ign->CntTimingOn);
			/*if(isDebug)
			{
				debug_transfer_new_message(&myDebug, Cyl->Tc_channel->TC_CV, "OnAt", *(Inj_or_Ign->TcCompareRegister));	
			}*/
		}
	}
	else if (!Inj_or_Ign->EventOnSameTooth && (Inj_or_Ign->ToothOff == CrankToothCounter)) //turn output OFF
	{
		if (Inj_or_Ign->CntTimingOff < PIOA_HANDLER_TIME_IN_COUNTS) // The interrupt vector does not react when a value is stored in compare register which has been reached in the end of this function/handler
		{
			/*if(isDebug)
			{
				debug_transfer_new_message(&myDebug, Cyl->Tc_channel->TC_CV, "Off", 0);	
			}*/
			Inj_or_Ign->pio->PIO_SODR = Inj_or_Ign->OutputPin;			// Sets pin to low
			Debug_Output->RealTimeTurnOffCount = Cyl->Tc_channel->TC_CV;
		}
		else
		{
			*(Inj_or_Ign->TcCompareRegister) = math_sum_with_overflow_protection(Cyl->Tc_channel->TC_CV, Inj_or_Ign->CntTimingOff);
			/*if(isDebug)
			{
				debug_transfer_new_message(&myDebug, Cyl->Tc_channel->TC_CV, "OffAt", *(Inj_or_Ign->TcCompareRegister));	
			}*/
		}
	}
}