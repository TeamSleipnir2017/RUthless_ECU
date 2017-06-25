/*
 * timers.c
 *
 * Created: 7.12.2016 18:27:39
 *  Author: jbbja
 */ 

#include "timers.h"

void timers_init1(void)
{
	/* Enable timer peripheral clock */
	PMC->PMC_PCER0 = (1 << ID_TC0);
	
	/* Initialize nested vector interrupts */
	/* Enable interrupt controller */
	/************************************************************************/
	/*
	TC  Chan    NVIC "irq"  IRQ handler    function PMC id
	TC0    0    TC0_IRQn    TC0_Handler    ID_TC0
	TC0    1    TC1_IRQn    TC1_Handler    ID_TC1
	TC0    2    TC2_IRQn    TC2_Handler    ID_TC2
	TC1    0    TC3_IRQn    TC3_Handler    ID_TC3
	TC1    1    TC4_IRQn    TC4_Handler    ID_TC4
	TC1    2    TC5_IRQn    TC5_Handler    ID_TC5
	TC2    0    TC6_IRQn    TC6_Handler    ID_TC6
	TC2    1    TC7_IRQn    TC7_Handler    ID_TC7
	TC2    2    TC8_IRQn    TC8_Handler    ID_TC8                                                                      */
	/************************************************************************/
	NVIC_DisableIRQ(TC0_IRQn);
	NVIC_ClearPendingIRQ(TC0_IRQn);
	NVIC_SetPriority(TC0_IRQn, 0); // ATHUGA SEINNA !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	NVIC_EnableIRQ(TC0_IRQn);
	
	/* TC interrupt Enable register */
	/************************************************************************/
	/* 
	COVFS: Counter Overflow
	CPAS: RA Compare
	CPBS: RB Compare
	CPCS: RC Compare
	                                                                     */
	/************************************************************************/
	TC0->TC_CHANNEL[0].TC_IER = TC_IER_COVFS | TC_IER_CPAS;
	
	/************************************************************************/
	/*  
	TIMER_CLOCK1 Clock selected: internal MCK/2 clock signal (from PMC)
	TIMER_CLOCK2 Clock selected: internal MCK/8 clock signal (from PMC)
	TIMER_CLOCK3 Clock selected: internal MCK/32 clock signal (from PMC)
	TIMER_CLOCK4 Clock selected: internal MCK/128 clock signal (from PMC)
	TIMER_CLOCK5 Clock selected: internal SLCK clock signal (from PMC)  
	XC0 Clock selected: XC0
	XC1 Clock selected: XC1
	XC2 Clock selected: XC2                                                                  */
	/************************************************************************/
	/* TC Channel mode register (MCK / 2) */
	TC0->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK1 | TC_CMR_WAVE;
	
	/* Load Register A to interrupt after specific counts */
	TC0->TC_CHANNEL[0].TC_RA = 420000000;
	
	/* TC channel control register, enable counter */
	TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

/* TimerChannel is 0-8 and priority is nested vector interrupt priority */
void timer_init(uint32_t TimerChannel, uint32_t TimerMode, uint32_t InterruptMode, uint8_t TimerInterruptPriority)
{
	// Check if the programmer is screwed
	if (TimerChannel > 8)
		return;
		
	// Enable peripheral clock
	pmc_enable_periph_clk(ID_TC0 + TimerChannel);
	
	Tc *Timer;
	// Initialize timer
	if (TimerChannel < 3) // TC0
		Timer = TC0; 
	else if (TimerChannel < 6) // TC1
		Timer = TC1;
	else // TC2
		Timer = TC2;
		
	tc_init(Timer, (TimerChannel%3), TimerMode);
	
	interrupts_enable_interrupt_vector(TC0_IRQn + TimerChannel, TimerInterruptPriority);
	
	tc_enable_interrupt(Timer, (TimerChannel%3), InterruptMode);
	
	tc_start(Timer, (TimerChannel%3));
	
	// TODO !!!!!!!!!!!!!
	TC8_Overflow = FALSE;
	millis = 0;
}

void timer_do_cylinder(uint8_t CylinderNr)
{
	//uart_transfer('a'); uart_print_int(CylinderNr); uart_new_line();
	struct cylinder_ *Cyl = &cylinder[CylinderNr];
	uint32_t CounterValue = Cyl->Tc_channel->TC_CV;
	uint32_t TimerStatus = Cyl->Tc_channel->TC_SR;
	
	if (TimerStatus & TC_SR_CPAS) // Compare register A ignition 1
	{
		/*if(isDebug)
		{
			debug_transfer_new_message(&myDebug, CounterValue, "doIgn", CylinderNr);	
		}*/
		timer_do_inj_or_ign(&cylinder[CylinderNr].Ign, &cylinder[CylinderNr], &debug_cylinders.ign_output_debug[CylinderNr]);
	}
	if (TimerStatus & TC_SR_CPBS) // Compare register B injector 1
	{
		/*if(isDebug)
		{
			debug_transfer_new_message(&myDebug, CounterValue, "doInj", CylinderNr);	
		}*/
		timer_do_inj_or_ign(&cylinder[CylinderNr].Inj, &cylinder[CylinderNr], &debug_cylinders.ign_output_debug[CylinderNr]);
	}
	if (TimerStatus & TC_SR_CPCS) // Compare register C injector 2
	{
	}
	if (TimerStatus & TC_SR_COVFS) // Overflow
	{ 
		// TODO: Necessary to handle the Overflow probably best to check when loading RA, RB and RC
	}
}

void timer_do_inj_or_ign(struct cylinder_output_manager *Inj_or_Ign, struct cylinder_ *Cyl, struct debug_cylinder_output_ *Debug_Output)
{
	if (Inj_or_Ign->EventPending)
	{
		/*if(isDebug)
		{
			debug_transfer_new_message(&myDebug, Cyl->Tc_channel->TC_CV, "TurnOn", 0);	
		}*/
		Inj_or_Ign->pio->PIO_CODR = Inj_or_Ign->OutputPin;			// Sets pin to high
		Inj_or_Ign->EventPending = FALSE;
		// Initiate output timeout
		*(Inj_or_Ign->TcCompareRegister) = math_sum_with_overflow_protection(Cyl->Tc_channel->TC_CV, Inj_or_Ign->CntTimeOutOff);
		Debug_Output->RealTimeTurnOnCount = Cyl->Tc_channel->TC_CV;
	}
	else
	{
		/*if(isDebug)
		{
			debug_transfer_new_message(&myDebug, Cyl->Tc_channel->TC_CV, "TurnOff", 0);	
		}*/
		Inj_or_Ign->pio->PIO_SODR = Inj_or_Ign->OutputPin;			// Sets pin to low
		Debug_Output->RealTimeTurnOffCount = Cyl->Tc_channel->TC_CV;
	}
	// TODO: NEEDS TO BE TESTED
	if (Inj_or_Ign->EventOnSameTooth)							// Check if Off event is at the same tooth
	{
		*(Inj_or_Ign->TcCompareRegister) = math_sum_with_overflow_protection(Cyl->Tc_channel->TC_CV, Inj_or_Ign->CntTimingOff - Inj_or_Ign->CntTimingOn);
		Inj_or_Ign->EventOnSameTooth = FALSE;
		/*if(isDebug)
		{
			debug_transfer_new_message(&myDebug, Cyl->Tc_channel->TC_CV, "TCSameTooth", *(Inj_or_Ign->TcCompareRegister));	
		}*/
	}
}

/*
Built in function
tc_read_ra (Tc *p_tc, uint32_t ul_channel)

tc_write_ra (Tc *p_tc, uint32_t ul_channel, uint32_t ul_value)

tc_enable_interrupt(Tc *p_tc, uint32_t ul_channel, uint32_t ul_sources)
*/

/* TC Status Register */
/************************************************************************/
/* By reading the register is every bit in it cleared
	COVFS: Counter Overflow Status
	CPAS: RA Compare Status
	CPBS: RB Compare Status 
	CPCS: RC Compare Status                                             */
/************************************************************************/

// Cylinder handlers
void TC0_Handler(void) { timer_do_cylinder(CYLINDER_1_TIMER); }
void TC1_Handler(void) { timer_do_cylinder(CYLINDER_2_TIMER); }
void TC2_Handler(void) { timer_do_cylinder(CYLINDER_3_TIMER); }
void TC3_Handler(void) { timer_do_cylinder(CYLINDER_4_TIMER); }
void TC4_Handler(void) { timer_do_cylinder(CYLINDER_5_TIMER); }
void TC5_Handler(void) { timer_do_cylinder(CYLINDER_6_TIMER); }
void TC6_Handler(void) { timer_do_cylinder(CYLINDER_7_TIMER); }
void TC7_Handler(void) { timer_do_cylinder(CYLINDER_8_TIMER); }

// GLOBAL_TIMER
void TC8_Handler(void)
{
	// Read the current TC8 Counter Value
	uint32_t CounterValue = TC2->TC_CHANNEL[2].TC_CV;
	// Read the current TC8 Status, Compare or overflow
	uint32_t TimerStatus = TC2->TC_CHANNEL[2].TC_SR;
	
	if (TimerStatus & TC_SR_CPAS) // Compare Register A ADC control
	{
		tc_write_ra(TC2, 2, CounterValue + GLOBAL_TIMER_FREQ/GlobalTimerFreqADCScaler);
		adc_start(ADC);
		//uart_transfer('a');
	}
	if (TimerStatus & TC_SR_CPBS) // Compare register B extra
	{
		TC2->TC_CHANNEL[2].TC_RB = CounterValue + GLOBAL_TIMER_FREQ/MILLI_SEC;
		//tc_write_rb(TC2, 2, CounterValue + GLOBAL_TIMER_FREQ/MILLI_SEC);
		//tc_write_rb(TC2, 2, CounterValue + GLOBAL_TIMER_FREQ/GlobalTimerFreqUARTScaler);
		// TODO: START UART
		//uart_transfer('b');
	}
	if (TimerStatus & TC_SR_CPCS) // Compare register C interrupt per millisec
	{
		tc_write_rc(TC2, 2, CounterValue + GLOBAL_TIMER_FREQ/MILLI_SEC);
		millis++;
		//tc_write_rc(TC2, 2, CounterValue + GLOBAL_TIMER_FREQ/GlobalTimerFreqTelemetryScaler);
	}
	if (TimerStatus & TC_SR_COVFS)
	{
		TC8_Overflow = TRUE;
		tc_write_ra(TC2, 2, GLOBAL_TIMER_FREQ/GlobalTimerFreqADCScaler);
		//tc_write_rb(TC2, 2, GLOBAL_TIMER_FREQ/GlobalTimerFreqUARTScaler);
		tc_write_rc(TC2, 2, GLOBAL_TIMER_FREQ/MILLI_SEC);
		// TODO MAYBE
	}
}
