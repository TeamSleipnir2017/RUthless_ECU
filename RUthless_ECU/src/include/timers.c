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
	TC2->TC_CHANNEL[2].TC_IER = TC_IER_CPBS;
	
	tc_start(Timer, (TimerChannel%3));
	
	// TODO !!!!!!!!!!!!!
	TC8_Overflow = FALSE;
	millis = 0;
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

// CYLINDER_1_TIMER
void TC0_Handler(void)
{
	if (DwellFirstFlag)
	{
		PIOC->PIO_SODR	=	IGN1_OUT;			// Sets pin PC19 to high
		DwellFirstFlag = FALSE;
	}
	else
	{
		PIOC->PIO_CODR	=	IGN1_OUT;			// Sets pin PC19 to low
	}
	uint32_t readtc	=	TC0->TC_CHANNEL[0].TC_SR;
	// TC0->TC_CHANNEL[0].TC_CCR	=	TC_CCR_CLKDIS;
}

// CYLINDER_2_TIMER
void TC1_Handler(void)
{
	if (DwellSecondFlag)	
	{
		PIOC->PIO_SODR	=	IGN2_OUT;			// Sets pin PC19 to high
		DwellSecondFlag = FALSE;
	}
	else
	{
		PIOC->PIO_CODR	=	IGN2_OUT;			// Sets pin PC19 to low
	}
	uint32_t readtc	=	TC0->TC_CHANNEL[1].TC_SR;
	TC0->TC_CHANNEL[1].TC_CCR	=	TC_CCR_CLKDIS;
}

// CYLINDER_3_TIMER
void TC2_Handler(void)
{
	if (DwellSecondFlag)
	{
		PIOC->PIO_SODR	=	IGN3_OUT;			// Sets pin PC17 to high
		DwellSecondFlag = FALSE;
	}
	else
	{
		PIOC->PIO_CODR	=	IGN3_OUT;			// Sets pin PC17 to low
	}
	uint32_t readtc	=	TC0->TC_CHANNEL[2].TC_SR;
	TC0->TC_CHANNEL[2].TC_CCR	=	TC_CCR_CLKDIS;
}

// CYLINDER_4_TIMER
void TC3_Handler(void)
{
	if (DwellFirstFlag)							
	{
		PIOC->PIO_SODR	=	IGN4_OUT;			// Sets pin PC25 to high
		DwellFirstFlag = FALSE;
	}
	else
	{
		PIOC->PIO_CODR	=	IGN4_OUT;			// Sets pin PC25 to low
	}
	uint32_t readtc	=	TC1->TC_CHANNEL[0].TC_SR;
	TC1->TC_CHANNEL[0].TC_CCR	=	TC_CCR_CLKDIS;
}
// CYLINDER_5_TIMER
void TC4_Handler(void)
{
	
}
// CYLINDER_6_TIMER
void TC5_Handler(void)
{
	
}
// CYLINDER_7_TIMER
void TC6_Handler(void)
{
	
}
// CYLINDER_8_TIMER
void TC7_Handler(void)
{
	
}
// GLOBAL_TIMER
void TC8_Handler(void)
{
	// Read the current TC8 Counter Value
	uint32_t CounterValue = tc_read_cv(TC2, 2);
	// Read the current TC8 Status, Compare or overflow
	uint32_t tc_status = TC2->TC_CHANNEL[2].TC_SR;
	if (tc_status & TC_SR_CPAS)
	{
		tc_write_ra(TC2, 2, CounterValue + GLOBAL_TIMER_FREQ/GlobalTimerFreqADCScaler);
		adc_start(ADC);
		//uart_transfer('a');
	}
	if (tc_status & TC_SR_CPBS) // Compare register B is not working ?? 25.2.17
	{
		TC2->TC_CHANNEL[2].TC_RB = CounterValue + GLOBAL_TIMER_FREQ/MILLI_SEC;
		//tc_write_rb(TC2, 2, CounterValue + GLOBAL_TIMER_FREQ/MILLI_SEC);
		//tc_write_rb(TC2, 2, CounterValue + GLOBAL_TIMER_FREQ/GlobalTimerFreqUARTScaler);
		// TODO: START UART
		//uart_transfer('b');
	}
	if (tc_status & TC_SR_CPCS)
	{
		tc_write_rc(TC2, 2, CounterValue + GLOBAL_TIMER_FREQ/MILLI_SEC);
		millis++;
		//tc_write_rc(TC2, 2, CounterValue + GLOBAL_TIMER_FREQ/GlobalTimerFreqTelemetryScaler);
	}
	if (tc_status & TC_SR_COVFS)
	{
		TC8_Overflow = TRUE;
		tc_write_ra(TC2, 2, GLOBAL_TIMER_FREQ/GlobalTimerFreqADCScaler);
		//tc_write_rb(TC2, 2, GLOBAL_TIMER_FREQ/GlobalTimerFreqUARTScaler);
		tc_write_rc(TC2, 2, GLOBAL_TIMER_FREQ/MILLI_SEC);
		// TODO MAYBE
	}
}
