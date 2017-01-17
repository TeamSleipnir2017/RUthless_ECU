/*
 * interrupts.c
 *
 * Created: 7.12.2016 14:31:53
 *  Author: jbbja
 */ 

#include "interrupts.h"

/* Initialize nested vector interrupts */
void enable_interrupt_vector(uint32_t irqn, uint32_t priority)
{
	NVIC_DisableIRQ(irqn);
	NVIC_ClearPendingIRQ(irqn);
	NVIC_SetPriority(irqn, priority); 
	NVIC_EnableIRQ(irqn);
}