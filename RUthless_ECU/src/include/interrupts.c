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
		CrankPrevCycleCounts	=		CrankCurrCycleCounts;
		CrankCurrCycleCounts	=		TimerCounterValue - CrankTimerCounts;
		CrankTimerCounts		=		TimerCounterValue;
		CrankRevCounts			+=		CrankCurrCycleCounts;
		CrankTooth++;
		CrankToothCounter++;
		CrankSignalFlag			=		TRUE;
		for (uint8_t i = 0; i < engine_config2.NrCylinders; i++)
		{
			interrupts_check_timer_for_inj_or_ign(&cylinder[i].Inj, &cylinder[i]);
			interrupts_check_timer_for_inj_or_ign(&cylinder[i].Ign, &cylinder[i]);
// 			struct cylinder_ *Cyl = &cylinder[i]; 
// 			if (Cyl->Inj.ToothOn == CrankToothCounter) // Compare register B is for injector (TC_RB)
// 			{
// 				if (Cyl->Inj.CntTimingOn < PIOA_HANDLER_TIME_IN_COUNTS) // The interrupt vector does not react when a value is stored in compare register which has been reached in the end of this function/handler
// 				{
// 					Cyl->Inj.pio->PIO_SODR = Cyl->Inj.OutputPin;			// Sets pin to high
// 					debug_cylinder[i].InjRealTimeTurnOnCount = Cyl->Tc_channel->TC_CV;
// 				}
// 				else
// 				{
// 					Cyl->Inj.EventPending = TRUE;
// 					Cyl->Tc_channel->TC_RB = math_sum_with_overflow_protection(Cyl->Tc_channel->TC_CV, Cyl->Inj.CntTimingOn);
// 				}
// 			}
// 			else if (!Cyl->Inj.EventOnSameTooth && (Cyl->Inj.ToothOff == CrankToothCounter)) // Compare register B is for injector (TC_RB)
// 			{
// 				if (Cyl->Inj.CntTimingOff < PIOA_HANDLER_TIME_IN_COUNTS) // The interrupt vector does not react when a value is stored in compare register which has been reached in the end of this function/handler
// 				{
// 					Cyl->Inj.pio->PIO_CODR = Cyl->Inj.OutputPin;			// Sets pin to low
// 					debug_cylinder[i].InjRealTimeTurnOffCount = Cyl->Tc_channel->TC_CV;
// 				}
// 				else
// 				{
// 					Cyl->Tc_channel->TC_RB = math_sum_with_overflow_protection(Cyl->Tc_channel->TC_CV, Cyl->Inj.CntTimingOff);
// 				}
// 			}
		}
// 		if (CrankTooth == DwellFirstTach)
// 		{
// 			DwellSecondFlag = TRUE;
// 			if (!CamSignalFlag)
// 			{
// 				TC0->TC_CHANNEL[0].TC_RA	=	TC0->TC_CHANNEL[0].TC_CV + DwellFirstInterval;
// 			}
// 			else
// 			{
// 				TC1->TC_CHANNEL[0].TC_RA	=	TC1->TC_CHANNEL[0].TC_CV + DwellFirstInterval;
// 			}
// 		}
// 		else if (CrankTooth == DwellSecondTach)
// 		{
// 			DwellFirstFlag = TRUE;
// 			
// 			if (!CamSignalFlag)
// 			{
// 				TC0->TC_CHANNEL[1].TC_RA	=	TC0->TC_CHANNEL[1].TC_CV + DwellSecondInterval;
// 			}
// 			else
// 			{
// 				TC0->TC_CHANNEL[2].TC_RA	=	TC0->TC_CHANNEL[2].TC_CV + DwellSecondInterval;
// 			}			
// 		}
// 		
// 		
// 		if (CrankTooth == CrankFirstTach)
// 		{
// 			if (!CamSignalFlag)
// 			{
// 				
// 				TC0->TC_CHANNEL[0].TC_RA	=	TC0->TC_CHANNEL[0].TC_CV + CrankFirstInterval;
// 			}
// 			else
// 			{
// 				TC1->TC_CHANNEL[0].TC_RA	=	TC1->TC_CHANNEL[0].TC_CV + CrankFirstInterval;
// 			}
// 		}
// 		else if (CrankTooth == CrankSecondTach)
// 		{
// 			if (!CamSignalFlag)
// 			{
// 				TC0->TC_CHANNEL[1].TC_RA	=	TC0->TC_CHANNEL[1].TC_CV + CrankSecondInterval;
// 			}
// 			else
// 			{
// 				TC0->TC_CHANNEL[2].TC_RA	=	TC0->TC_CHANNEL[2].TC_CV + CrankSecondInterval;
// 			}
// 		}
	}
	
	
	
	// Check if the interrupt source is from the camshaft sensor
	if (status_register & CAM_SIGNAL)
	{
		CamCurrCycleCounts		=		TimerCounterValue - CamTimerCounts;
		CamTimerCounts			=		TimerCounterValue;
		CamSignalFlag			=		TRUE;
	}
	
	PIOAHandlerTimeInCounts = TC2->TC_CHANNEL[2].TC_CV - TimerCounterValue; 
}

void interrupts_check_timer_for_inj_or_ign(struct cylinder_output_manager *Inj_or_Ign, struct cylinder_ *Cyl)
{
	if (Inj_or_Ign->ToothOn == CrankToothCounter) 
	{
		if (Inj_or_Ign->CntTimingOn < PIOA_HANDLER_TIME_IN_COUNTS) // The interrupt vector does not react when a value is stored in compare register which has been reached in the end of this function/handler
		{
			Inj_or_Ign->pio->PIO_CODR = Inj_or_Ign->OutputPin;			// Sets pin to high
			//debug_cylinder[i].InjRealTimeTurnOnCount = Cyl->Tc_channel->TC_CV;
		}
		else
		{
			Inj_or_Ign->EventPending = TRUE;
			*(Inj_or_Ign->TcCompareRegister) = math_sum_with_overflow_protection(Cyl->Tc_channel->TC_CV, Inj_or_Ign->CntTimingOn);
		}
	}
	else if (!Inj_or_Ign->EventOnSameTooth && (Inj_or_Ign->ToothOff == CrankToothCounter))
	{
		if (Inj_or_Ign->CntTimingOff < PIOA_HANDLER_TIME_IN_COUNTS) // The interrupt vector does not react when a value is stored in compare register which has been reached in the end of this function/handler
		{
			Inj_or_Ign->pio->PIO_SODR = Inj_or_Ign->OutputPin;			// Sets pin to low
			//debug_cylinder[i].InjRealTimeTurnOffCount = Cyl->Tc_channel->TC_CV;
		}
		else
		{
			*(Inj_or_Ign->TcCompareRegister) = math_sum_with_overflow_protection(Cyl->Tc_channel->TC_CV, Inj_or_Ign->CntTimingOff);
		}
	}
}